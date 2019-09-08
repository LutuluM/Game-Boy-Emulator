#include "Graphics.h"
#include <time.h>
#include "CPU.h"
#include "Memory.h"
#include "IOs.h"
#include <SDL.h>


SDL_Window * window, * Tiles;
SDL_Renderer * renderer, * rend;
uchar vBuffer[144][160][3];
uchar sl = 0;
uchar framesPerSecond = 0;


///SDL Grpahics Handlers 
void initGraphics() {

	SDL_Init(SDL_INIT_EVERYTHING);

	window = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Winw, Winh, SDL_WINDOW_VULKAN);//SDL_WINDOW_OPENGL);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	/*Tiles = SDL_CreateWindow("Tiles", 809 + Winw, 10, 16 * 8 * 2, 12 * 8 * 2, SDL_WINDOW_OPENGL);
	rend = SDL_CreateRenderer(Tiles, -1, SDL_RENDERER_ACCELERATED);
	
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);*/
}

void exitGraphics() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(Tiles);
	SDL_Quit();
}

///Updates VideoBuffer with new scanline values. Prints frame when scanline resets. Deals with Background and Window
void NextGraphic()
{
	static ulong videoTicks = 0;
	static uchar Mode = 0;
	ulong elapsedTime;

	static clock_t t1 = clock();
	static uchar fps = 0;
	static ushort frames = 0;

	//while (1) 
	{
		//do{
			if (clock() - t1 > 1000)
			{
				t1 = clock();
				framesPerSecond = fps;
				printf("FPS: %d\n", fps);
				//printf("%d", getCPUTicks());
				fps = 0;
			}
		//} while (fps > 59);//frame locking*/

		elapsedTime = getCPUTicks() - videoTicks;
		
		switch (Mode)//2->3->0->1
		{
			case 0://h-blank 204
				if (elapsedTime > 203)
				{
					if ((readMem(0xFF40) & 0x80) != 0x80)//lcd off
					{
						videoTicks += 204;
						//Mode = 1;
						break;
					}

					printBackGroundTiles();	//prints background for current scanline
					printWindowTiles(); //print window for current scanline
					printSprites(); //prints sprites to buffer of current scanline
					//printBuffer();	//prints current scanlin of screen buffer, replaced with full frame printing for being slow
					
					sl++;
					if (sl >= Winh) //V-Blank start
					{
						writeMem(0xFF0F, readMem(0xFF0F) | 1);
						printFrame(); //prints a full frame
						frames++;
						fps++;
						Mode = 1;
					}
					else//Next Line
						Mode = 2;
						
					writeMem(0xFF44, sl);//update LCD Y-Coordinate
					videoTicks += 204;
				}
				break;
			case 1://v-blank 4560
				if ((readMem(0xFF40) & 0x80) == 0x80)//lcd on
				{
					if (elapsedTime > 455)
					{
						sl++;
						if (sl == Vy)
						{
							sl = 0;
							Mode = 2;
						}
						writeMem(0xFF44, sl);
						videoTicks += 456;
					}
				}
				break;
			case 2://transfer 80
				if (elapsedTime > 79)
				{
					videoTicks += 80;
					Mode = 3;
				}
				break;
			case 3://search 172
				if (elapsedTime > 171)
				{
					videoTicks += 172;
					Mode = 0;
				}
				break;
		}
		setLCDC((readMem(0xFF41) & 0xFC) | Mode); //change videomode
	}
	if (getPC() == 0x100)
	{
		printf("Frame Count: %d\n", frames);
	}
}
//Prints a row of pixels for the Background to the video buffer
void printBackGroundTiles() {
	uchar tile, C1, C2, p1, p2, p3, b, x, y, selectTile, selectData;

	if (sl > Winh-1)
		return;

	y = ((sl + yoff) / 8) % 32;//determines which y tile get read
	b = readMem(0xFF47);//get background color

	selectTile = (readMem(0xFF40) & 0x8);
	selectData = (readMem(0xFF40) & 0x10);


	for (x = 0; x<Winw && (sl<Winh); x++)
	{
		// Select tile map to be used
		if (selectTile == 0x8)
			tile = readMem(0x9C00 + ((x + xoff)/8)%32 + y * 32);
		else
			tile = readMem(0x9800 + ((x + xoff)/8)%32 + y * 32);

		//select tile design location
		if (selectData == 0x10)
		{
			C1 = readMem(0x8000 + tile * 16 + ((sl + yoff) % 8) * 2);
			C2 = readMem(0x8000 + tile * 16 + ((sl + yoff) % 8) * 2 + 1);
		}
		else //set up shifted pallete here
		{
			C1 = readMem(0x9000 + (char)tile * 16 + ((sl + yoff) % 8) * 2);
			C2 = readMem(0x9000 + (char)tile * 16 + ((sl + yoff) % 8) * 2 + 1);
		}
			p1 = (C1 >> (7 - (x + xoff)%8)) & 0x1;
			p2 = (C2 >> (7 - (x + xoff)%8)) & 0x1;
			p3 = (p2 << 1) | p1;
			p3 = (b >> 2 * p3) & 0x3;
			switch (p3)
			{
			case 3: 
				vBuffer[sl][x][0] = 0x20;//black
				vBuffer[sl][x][1] = 0x18;
				vBuffer[sl][x][2] = 0x08;
				break;
			case 2: 
				vBuffer[sl][x][0] = 0x56;
				vBuffer[sl][x][1] = 0x68;
				vBuffer[sl][x][2] = 0x34;
				break;
			case 1:
				vBuffer[sl][x][0] = 0x70;
				vBuffer[sl][x][1] = 0xC0;
				vBuffer[sl][x][2] = 0x88;
				break;
			default: 
				vBuffer[sl][x][0] = 0xD0;
				vBuffer[sl][x][1] = 0xF8;
				vBuffer[sl][x][2] = 0xE0;//white
			}
	}
}
//Prints a row of pixels for the Window.
void printWindowTiles() {//reorder for speed or leave it as a more read able section
	uchar tile, C1, C2, p1, p2, p3, b, x, y, selectTile, selectData;
	uchar yoffset, xoffset;

	yoffset = readMem(0xFF4A);//window offsets
	xoffset = readMem(0xFF4B);//x-7

	if (sl - yoffset < 0)
		return;
	if ((readMem(0xFF40) & 0x20) == 0) //background disabled
		return;

	y = ((sl - yoffset) / 8) % 32;//determines which y tile get read
	b = readMem(0xFF47);//get window color palette

	selectTile = (readMem(0xFF40) & 0x40);
	selectData = (readMem(0xFF40) & 0x10);


	for (x = 0; x<Winw && sl<Winh; x++)
	{
		if (x + xoffset < 7)
			continue;
		// Select tile map to be used
		if (selectTile == 0x40)
			tile = readMem(0x9C00 + ((x - 7 + xoffset)/8)%32 + y * 32);
		else
			tile = readMem(0x9800 + ((x - 7 + xoffset)/8)%32 + y * 32);

		//select tile design location
		if (selectData == 0x10)
		{
			C1 = readMem(0x8000 + tile * 16 + ((sl - yoffset) % 8) * 2);
			C2 = readMem(0x8000 + tile * 16 + ((sl - yoffset) % 8) * 2 + 1);
		}
		else //set up shifted pallete here
		{
			C1 = readMem(0x9000 + (char)tile * 16 + ((sl - yoffset) % 8) * 2);
			C2 = readMem(0x9000 + (char)tile * 16 + ((sl - yoffset) % 8) * 2 + 1);
		}
			p1 = (C1 >> (7 - (x-7 + xoffset)%8)) & 0x1;
			p2 = (C2 >> (7 - (x-7 + xoffset)%8)) & 0x1;
			p3 = (p2 << 1) | p1;
			p3 = (b >> 2 * p3) & 0x3;
			switch (p3)
			{
			case 3: 
				vBuffer[sl][x][0] = 0x20;//black
				vBuffer[sl][x][1] = 0x18;
				vBuffer[sl][x][2] = 0x08;
				break;
			case 2: 
				vBuffer[sl][x][0] = 0x56;
				vBuffer[sl][x][1] = 0x68;
				vBuffer[sl][x][2] = 0x34;
				break;
			case 1:
				vBuffer[sl][x][0] = 0x70;
				vBuffer[sl][x][1] = 0xC0;
				vBuffer[sl][x][2] = 0x88;
				break;
			default: 
				vBuffer[sl][x][0] = 0xD0;
				vBuffer[sl][x][1] = 0xF8;
				vBuffer[sl][x][2] = 0xE0;
				//white
			}
	}
}
//Prints single line to screen. Obsolete for now
void printBuffer() {
	uchar x;
	for (x = 0; (x < Winw) && (sl < Winh); x++)
	{
		SDL_SetRenderDrawColor(renderer, vBuffer[sl][x][0], vBuffer[sl][x][1], vBuffer[sl][x][2], 0xFF);
		SDL_RenderDrawPoint(renderer, x, sl);
	}
	SDL_RenderPresent(renderer);
}
//Prints entire frame to screen
void printFrame(){
	uchar x,y;
	for (x = 0; x < Winw; x++)
		for(y = 0; y < Winh; y++)
		{
			SDL_SetRenderDrawColor(renderer, vBuffer[y][x][0], vBuffer[y][x][1], vBuffer[y][x][2], 0xFF);
			SDL_RenderDrawPoint(renderer, x, y);
		}
	SDL_RenderPresent(renderer);
}



///Deals with Sprite Rendering
void printSprites() {
	uchar tile, x, y, attr, colorPalette, prio;
	uchar i;
		if ((readMem(0xFF40) & 0x2) != 0x2)
			return;//sprites disabled
		if (sl > Winh-1)//sl off screen
			return;
		for (i = 0; i < 40; i++) // Loop through all sprites
		{
			y = readMem(0xFE00 + 4 * i);		//sprite y pixel coordinate
			x = readMem(0xFE00 + 4 * i + 1);	//sprite x pixel coordinate

			if ((x == 0 || x > 167) || (y == 0 || y > 159)) //skips sprite if it is off screen
				continue;
			if (y - 16 > sl || sl > y-9)					//skips sprite if sl isn't on the sprite
				continue;

			tile = readMem(0xFE00 + 4 * i + 2);	//Get sprite tile number/Image Data Location
			attr = readMem(0xFE00 + 4 * i + 3);	//Sprite attributes
			if ((attr & 0x10) == 0x10)
				colorPalette = readMem(0xFF49); //Use Palette OBJ1
			else
				colorPalette = readMem(0xFF48); //Use Palette OBJ0
			prio = attr >> 7;

			switch ((attr >> 5) & 0x3)
			{
			case 0:
				flipSpriteN(tile, colorPalette, x, y, prio);
				break;
			case 1:
				flipSpriteV(tile, colorPalette, x, y, prio);
				break;
			case 2:
				flipSpriteH(tile, colorPalette, x, y, prio);
				break;
			case 3:
				flipSpriteVH(tile, colorPalette, x, y, prio);
				break;
			}
		}
}

void flipSpriteN(uchar tile, uchar colorPalette, uchar x1, uchar y1,uchar prio) {
	uchar y, x, p1, p2, p3, C1, C2;
	
		y = sl - (y1 - 16); //select the current row of the scanline
		C1 = readMem(0x8000 + tile * 16 + y * 2);		//Get Color deisgn for the scanline
		C2 = readMem(0x8000 + tile * 16 + y * 2 + 1);	//Each line requires 2 bytes

		for (x = 0; x < 8; x++)
		{
			if ((x1 + x < 8) || ((x1 + x) >= 167))//skip pixels offscreen
				continue;
			p1 = (C1 >> (7 - x)) & 0x1;
			p2 = (C2 >> (7 - x)) & 0x1;
			p3 = (p2 << 1) | p1;
			//p3 is the color pattern for the sprite

			if (p3 == 0)
				continue;//color 0 is skipped
			p3 = (colorPalette >> 2 * p3) & 0x3;

			uchar newX = (x1 + x - 8);

			if (prio && (vBuffer[sl][newX][0] != 0xD0)) //if the backgound isn't white
				continue;
			
			switch (p3)
			{
			case 3:
				vBuffer[sl][newX][0] = 0x20;//black
				vBuffer[sl][newX][1] = 0x18;
				vBuffer[sl][newX][2] = 0x08;
				break;
			case 2:
				vBuffer[sl][newX][0] = 0x56;
				vBuffer[sl][newX][1] = 0x68;
				vBuffer[sl][newX][2] = 0x34;
				break;
			case 1:
				vBuffer[sl][newX][0] = 0x70;
				vBuffer[sl][newX][1] = 0xC0;
				vBuffer[sl][newX][2] = 0x88;
				break;
			default:
				vBuffer[sl][x1 + x - 8][0] = 0xD0;
				vBuffer[sl][x1 + x - 8][1] = 0xF8;
				vBuffer[sl][x1 + x - 8][2] = 0xE0;
				break;//white
			}
		}
}

void flipSpriteV(uchar tile, uchar colorPalette, uchar x1, uchar y1,uchar prio) {
	uchar y, x, p1, p2, p3, C1, C2;

		y = sl - (y1 - 16);
		C1 = readMem(0x8000 + tile * 16 + y * 2);	// design data 2 bytes
		C2 = readMem(0x8000 + tile * 16 + y * 2 + 1);

		for (x = 0; x < 8; x++)
		{
			if ((x1 + x < 8) || ((x1 + x) >= 168))
				continue;
			p1 = (C1 >> (x)) & 0x1;
			p2 = (C2 >> (x)) & 0x1;
			p3 = (p2 << 1) | p1;

			if (p3 == 0)
				continue;

			p3 = (colorPalette >> 2 * p3) & 0x3;

			uchar newX = (x1 + x - 8);

			if (prio && (vBuffer[sl][x1 + x - 8][0] != 0xD0)) //if the backgound isn't white
				continue;

			switch (p3)
			{
			case 3:
				vBuffer[sl][x1 + x - 8][0] = 0x20;//black
				vBuffer[sl][x1 + x - 8][1] = 0x18;
				vBuffer[sl][x1 + x - 8][2] = 0x08;
				break;
			case 2:
				vBuffer[sl][x1 + x - 8][0] = 0x56;
				vBuffer[sl][x1 + x - 8][1] = 0x68;
				vBuffer[sl][x1 + x - 8][2] = 0x34;
				break;
			case 1:
				vBuffer[sl][x1 + x - 8][0] = 0x70;
				vBuffer[sl][x1 + x - 8][1] = 0xC0;
				vBuffer[sl][x1 + x - 8][2] = 0x88;
				break;
			case 0:
				vBuffer[sl][x1 + x - 8][0] = 0xD0;
				vBuffer[sl][x1 + x - 8][1] = 0xF8;
				vBuffer[sl][x1 + x - 8][2] = 0xE0;
				break;//white
			}
		}

}

void flipSpriteH(uchar tile, uchar colorPalette, uchar x1, uchar y1, uchar prio) {
	uchar y, x, p1, p2, p3, C1, C2;

		y = sl - (y1 - 16);
		C1 = readMem(0x8000 + tile * 16 + (7-y) * 2);	// design data 2 bytes
		C2 = readMem(0x8000 + tile * 16 + (7-y) * 2 + 1);

		for (x = 0; x < 8; x++)
		{
			if ((x1 + x < 8) || ((x1 + x) >= 168))
				continue;
			p1 = (C1 >> (7 - x)) & 0x1;
			p2 = (C2 >> (7 - x)) & 0x1;
			p3 = (p2 << 1) | p1;
			if (p3 == 0)
				continue;

			p3 = (colorPalette >> 2 * p3) & 0x3;

			uchar newX = (x1 + x - 8);
			
			if (prio && (vBuffer[sl][x1 + x - 8][0] != 0xD0)) //if the backgound isn't white
				continue;

			switch (p3)
			{
			case 3://y1 + (7 - y) - 16
				vBuffer[sl][x1 + x - 8][0] = 0x20;//black
				vBuffer[sl][x1 + x - 8][1] = 0x18;
				vBuffer[sl][x1 + x - 8][2] = 0x08;
				break;
			case 2:
				vBuffer[sl][x1 + x - 8][0] = 0x56;
				vBuffer[sl][x1 + x - 8][1] = 0x68;
				vBuffer[sl][x1 + x - 8][2] = 0x34;
				break;
			case 1:
				vBuffer[sl][x1 + x - 8][0] = 0x70;
				vBuffer[sl][x1 + x - 8][1] = 0xC0;
				vBuffer[sl][x1 + x - 8][2] = 0x88;
				break;
			case 0:
				vBuffer[sl][x1 + x - 8][0] = 0xD0;
				vBuffer[sl][x1 + x - 8][1] = 0xF8;
				vBuffer[sl][x1 + x - 8][2] = 0xE0;
				break;//white
			}
		}
}

void flipSpriteVH(uchar tile, uchar colorPalette, uchar x1, uchar y1, uchar prio) {
	uchar y, x, p1, p2, p3, C1, C2;

		y = sl - y1 + 16;
		C1 = readMem(0x8000 + tile * 16 + (7-y) * 2);	// design data 2 bytes
		C2 = readMem(0x8000 + tile * 16 + (7-y) * 2 + 1);

		for (x = 0; x < 8; x++)
		{
			if ((x1 + x < 8) || ((x1 + x) >= 168))
				continue;
			p1 = (C1 >> (x)) & 0x1;
			p2 = (C2 >> (x)) & 0x1;
			p3 = (p2 << 1) | p1;
			if (p3 == 0)
				continue;

			p3 = (colorPalette >> 2 * p3) & 0x3;

			uchar newX = (x1 + x - 8);

			if (prio && (vBuffer[sl][x1 + x - 8][0] != 0xD0)) //if the backgound isn't white
				continue;

			switch (p3)
			{
			case 3:
				vBuffer[sl][x1 + x - 8][0] = 0x20;//black
				vBuffer[sl][x1 + x - 8][1] = 0x18;
				vBuffer[sl][x1 + x - 8][2] = 0x08;
				break;
			case 2:
				vBuffer[sl][x1 + x - 8][0] = 0x56;
				vBuffer[sl][x1 + x - 8][1] = 0x68;
				vBuffer[sl][x1 + x - 8][2] = 0x34;
				break;
			case 1:
				vBuffer[sl][x1 + x - 8][0] = 0x70;
				vBuffer[sl][x1 + x - 8][1] = 0xC0;
				vBuffer[sl][x1 + x - 8][2] = 0x88;
				break;
			case 0:
				vBuffer[sl][x1 + x - 8][0] = 0xD0;
				vBuffer[sl][x1 + x - 8][1] = 0xF8;
				vBuffer[sl][x1 + x - 8][2] = 0xE0;
				break;//white
			}
		}
}



///Debugging Graphics
//Prints ALL tiles and sprites to screen
void printMemory(){
	//static clock_t t2 = clock(); //update this every 1/4 sec
	static ulong t2 = 0;
	//t2++;
	while (1)
	{
		//if ((clock() - t2) >= 250)
		if(t2 > 250000)//~4hz at full speed if running full speed
		{
			t2 = 0;
			printVTable();	//Prints Video ram table out for debug
			printSTable();	//Prints out all the sprites for debug
			SDL_RenderPresent(rend);
			//printf("New render to Debugger\n");
		}
		t2++;
	}
}
//Prints VRam table to screen 16x12 grid
void printVTable() {
	uchar C1, C2, p1, p2, p3, b, x, y, i, j, off;
	b = readMem(0xFF47);//get background color
	off = 0;
	for (y = 0; y < 12 * 2; y++)
	{
		if (y == 12)
			off = 16 * 8;
		for (x = 0; x < 16; x++)
		{
			for (i = 0; i < 8; i++)
			{
				C1 = readMem(0x8000 + ((y * 16 * 8) + x * 8 + i) * 2);
				C2 = readMem(0x8000 + ((y * 16 * 8) + x * 8 + i) * 2 + 1);

				for (j = 0; j < 8; j++)
				{
					p1 = (C1 >> (7 - j)) & 0x1;
					p2 = (C2 >> (7 - j)) & 0x1;
					p3 = (p2 << 1) | p1;
					p3 = (b >> 2 * p3) & 0x3;
					switch (p3)
					{
					case 3: SDL_SetRenderDrawColor(rend, 0x20, 0x18, 0x08, 0xFF); break;//Black
					case 2: SDL_SetRenderDrawColor(rend, 0x56, 0x68, 0x34, 0xFF); break;
					case 1: SDL_SetRenderDrawColor(rend, 0x70, 0xC0, 0x88, 0xFF); break;
					case 0: SDL_SetRenderDrawColor(rend, 0xD0, 0xF8, 0xE0, 0xFF); break;//White
					}
					SDL_RenderDrawPoint(rend, (x * 8) + j + off, (y * 8) + i);
				}
			}
		}
	}
}
//Prints All sprites to screen
void printSTable() {
	uchar tile, C1, C2, p1, p2, p3, b, x, y, attr;
	uchar i, j, k;

	for (k = 0; k < 40; k++) // Loop through all sprites
	{
		x = k % 16;
		y = k / 16;
		tile = readMem(0xFE00 + 4 * k + 2);	//Get sprite tile number
		attr = readMem(0xFE00 + 4 * k + 3);	//Sprite attributes
		if ((attr & 0x10) == 0)
			b = readMem(0xFF48); //get palette for sprite
		else
			b = readMem(0xFF49); //get palette for sprite
		for (i = 0; i < 8; i++)

		{
			C1 = readMem(0x8000 + tile * 16 + i * 2);	// design data 2 bytes
			C2 = readMem(0x8000 + tile * 16 + i * 2 + 1);

			for (j = 0; j < 8; j++)
			{
				p1 = (C1 >> (7 - j)) & 0x1;
				p2 = (C2 >> (7 - j)) & 0x1;
				p3 = (p2 << 1) | p1;
				p3 = (b >> 2 * p3) & 0x3;
				switch (p3)
				{
				case 3: SDL_SetRenderDrawColor(rend, 0x20, 0x18, 0x08, 0xFF); break;//Black
				case 2: SDL_SetRenderDrawColor(rend, 0x56, 0x68, 0x34, 0xFF); break;
				case 1: SDL_SetRenderDrawColor(rend, 0x70, 0xC0, 0x88, 0xFF); break;
				case 0: SDL_SetRenderDrawColor(rend, 0xD0, 0xF8, 0xE0, 0xFF); break;
				}
				SDL_RenderDrawPoint(rend, 16 * 8 + j + x * 8, i + y * 8);
			}
		}
	}
}

uchar getSL() {
	return sl;
}

uchar getfps() {
	return framesPerSecond;
}