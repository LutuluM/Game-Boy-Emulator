#include "Graphics.h"
#include "time.h"
#include "CPU.h"
#include <SDL.h>

uchar sl = 0;
extern GBCPU CPU;
SDL_Window * window, *Tiles;
SDL_Renderer * renderer, *rend;
uchar vBuffer[144][160][3];

void initGraphics() {

	SDL_Init(SDL_INIT_EVERYTHING);

	window = SDL_CreateWindow("Test", 800, 10, Winw, Winh, SDL_WINDOW_OPENGL);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	Tiles = SDL_CreateWindow("Tiles", 809 + Winw, 10, 16 * 8 * 2, 12 * 8 * 2, SDL_WINDOW_OPENGL);
	rend = SDL_CreateRenderer(Tiles, -1, SDL_RENDERER_ACCELERATED);
}

void exitGraphics() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(Tiles);
	SDL_Quit();
}


//Prints lines pixels at a time
void NextGraphic()
{
	static unsigned int videoTicks = 0;
	uchar elapsedTime;

	if (videoTicks > CPU.CPUTicks)
		elapsedTime = videoTicks - CPU.CPUTicks;
	else
		elapsedTime = CPU.CPUTicks - videoTicks;
	

	if (elapsedTime >= Vx)
	{
		if ((readMem(0xFF40) & 0x80) == 0x80)//lcd on
		{
			printPixel(); //Not updated yet
			printSprites();
			printBuffer();
		}
		videoTicks = CPU.CPUTicks;
	}
}

//prints all Sprites to buffer
void printSprites() {
	uchar tile, x1, y1, attr, b;
	uchar k;

	if ((readMem(0xFF40) & 0x2) != 0x2)
		return; //sprites disabled

	for (k = 0; k < 40; k++) // Loop through all sprites
	{
		y1 = readMem(0xFE00 + 4 * k);		//sprite y
		x1 = readMem(0xFE00 + 4 * k + 1);	//spiite x

		if ((x1 == 0 || x1 > 167) || (y1 == 0 || y1 > 159))
			continue;

		tile = readMem(0xFE00 + 4 * k + 2);	//Get sprite tile number
		attr = readMem(0xFE00 + 4 * k + 3);	//Sprite attributes
		if ((attr & 0x10) == 0)
			b = readMem(0xFF48); //get palette for sprite
		else
			b = readMem(0xFF49); //get palette for sprite

		switch ((attr >> 5) & 0x3)
		{
			default:
				flipSpriteN(tile, b, x1, y1);
				break;
			case 1:
				flipSpriteV(tile, b, x1, y1);
				break;
			case 2:
				flipSpriteH(tile, b, x1, y1);
				break;
			case 3:
				flipSpriteVH(tile, b, x1, y1);
				break;
		}
	}
}

//Prints a scanline to buffer
void printPixel() {
	uchar tile, C1, C2, p1, p2, p3, b, x, y, j, selectTile, selectData;

	if (sl > Winh)
		return;

	y = ((sl + yoff) / 8) % 32;
	b = readMem(0xFF47);//get background color

	selectTile = (readMem(0xFF40) & 0x8);
	selectData = (readMem(0xFF40) & 0x10);


	for (x = 0; x<(Winw / 8) && (sl<Winh); x++)
	{
		// Select tile map to be used
		if (selectTile == 0x8)
			tile = readMem(0x9C00 + (x + xoff) + y * 32);
		else
			tile = readMem(0x9800 + (x + xoff) + y * 32);

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

		for (j = 0; j<8; j++)
		{
			p1 = (C1 >> (7 - j)) & 0x1;
			p2 = (C2 >> (7 - j)) & 0x1;
			p3 = (p2 << 1) | p1;
			p3 = (b >> 2 * p3) & 0x3;
			switch (p3)
			{
			case 3: 
				vBuffer[sl][(x * 8) + j][0] = 0x20;//black
				vBuffer[sl][(x * 8) + j][1] = 0x18;
				vBuffer[sl][(x * 8) + j][2] = 0x08;
				break;
			case 2: 
				vBuffer[sl][(x * 8) + j][0] = 0x56;
				vBuffer[sl][(x * 8) + j][1] = 0x68;
				vBuffer[sl][(x * 8) + j][2] = 0x34;
				break;
			case 1:
				vBuffer[sl][(x * 8) + j][0] = 0x70;
				vBuffer[sl][(x * 8) + j][1] = 0xC0;
				vBuffer[sl][(x * 8) + j][2] = 0x88;
				break;
			case 0: 
				vBuffer[sl][(x * 8) + j][0] = 0xD0;
				vBuffer[sl][(x * 8) + j][1] = 0xF8;
				vBuffer[sl][(x * 8) + j][2] = 0xE0;//white
			}
		}
	}
}
//Renders current scanline
void printBuffer() {
	uchar x;
	for (x = 0; (x < Winw) && (sl<Winh); x++)
	{
		SDL_SetRenderDrawColor(renderer, vBuffer[sl][x][0], vBuffer[sl][x][1], vBuffer[sl][x][2], 0xFF);
		SDL_RenderDrawPoint(renderer, x, sl);
	}
	sl = (++sl) % Vy;
	if (sl == Winh) writeMem(0xFF0F, readMem(0xFF0F) | 1, 1);
	writeMem(0xFF44, sl, 1);//update LCD Y-Coordinate
	SDL_RenderPresent(renderer);
}

void flipSpriteN(uchar tile, uchar b, uchar x1, uchar y1) {
	uchar i, j, p1, p2, p3, C1, C2;

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
			case 3:
				vBuffer[y1 + i - 16][x1 + j - 8][0] = 0x20;//black
				vBuffer[y1 + i - 16][x1 + j - 8][1] = 0x18;
				vBuffer[y1 + i - 16][x1 + j - 8][2] = 0x08;
				break;
			case 2:
				vBuffer[y1 + i - 16][x1 + j - 8][0] = 0x56;
				vBuffer[y1 + i - 16][x1 + j - 8][1] = 0x68;
				vBuffer[y1 + i - 16][x1 + j - 8][2] = 0x34;
				break;
			case 1:
				vBuffer[y1 + i - 16][x1 + j - 8][0] = 0x70;
				vBuffer[y1 + i - 16][x1 + j - 8][1] = 0xC0;
				vBuffer[y1 + i - 16][x1 + j - 8][2] = 0x88;
				break;
			case 0:
				break; //transparent
			}
		}
	}
}

void flipSpriteV(uchar tile,uchar b, uchar x1, uchar y1) {
	uchar i, j, p1, p2, p3, C1, C2;

	for (i = 0; i < 8; i++)

	{
		C1 = readMem(0x8000 + tile * 16 + i * 2);	// design data 2 bytes
		C2 = readMem(0x8000 + tile * 16 + i * 2 + 1);

		for (j = 0; j < 8; j++)
		{
			p1 = (C1 >> (j)) & 0x1;
			p2 = (C2 >> (j)) & 0x1;
			p3 = (p2 << 1) | p1;
			p3 = (b >> 2 * p3) & 0x3;
			switch (p3)
			{
			case 3:
				vBuffer[y1 + i - 16][x1 + j - 8][0] = 0x20;//black
				vBuffer[y1 + i - 16][x1 + j - 8][1] = 0x18;
				vBuffer[y1 + i - 16][x1 + j - 8][2] = 0x08;
				break;
			case 2:
				vBuffer[y1 + i - 16][x1 + j - 8][0] = 0x56;
				vBuffer[y1 + i - 16][x1 + j - 8][1] = 0x68;
				vBuffer[y1 + i - 16][x1 + j - 8][2] = 0x34;
				break;
			case 1:
				vBuffer[y1 + i - 16][x1 + j - 8][0] = 0x70;
				vBuffer[y1 + i - 16][x1 + j - 8][1] = 0xC0;
				vBuffer[y1 + i - 16][x1 + j - 8][2] = 0x88;
				break;
			case 0:
				break; //transparent
			}
		}
	}

}

void flipSpriteH(uchar tile, uchar b, uchar x1, uchar y1) {
	uchar i, j, p1, p2, p3, C1, C2;

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
			case 3:
				vBuffer[y1 + (7 - i) - 16][x1 + j - 8][0] = 0x20;//black
				vBuffer[y1 + (7 - i) - 16][x1 + j - 8][1] = 0x18;
				vBuffer[y1 + (7 - i) - 16][x1 + j - 8][2] = 0x08;
				break;
			case 2:
				vBuffer[y1 + (7 - i) - 16][x1 + j - 8][0] = 0x56;
				vBuffer[y1 + (7 - i) - 16][x1 + j - 8][1] = 0x68;
				vBuffer[y1 + (7 - i) - 16][x1 + j - 8][2] = 0x34;
				break;
			case 1:
				vBuffer[y1 + (7 - i) - 16][x1 + j - 8][0] = 0x70;
				vBuffer[y1 + (7 - i) - 16][x1 + j - 8][1] = 0xC0;
				vBuffer[y1 + (7 - i) - 16][x1 + j - 8][2] = 0x88;
				break;
			case 0:
				break; //transparent
			}
		}
	}
}

void flipSpriteVH(uchar tile, uchar b, uchar x1, uchar y1) {
	uchar i, j, p1, p2, p3, C1, C2;

	for (i = 0; i < 8; i++)

	{
		C1 = readMem(0x8000 + tile * 16 + i * 2);	// design data 2 bytes
		C2 = readMem(0x8000 + tile * 16 + i * 2 + 1);

		for (j = 0; j < 8; j++)
		{
			p1 = (C1 >> (j)) & 0x1;
			p2 = (C2 >> (j)) & 0x1;
			p3 = (p2 << 1) | p1;
			p3 = (b >> 2 * p3) & 0x3;
			switch (p3)
			{
			case 3:
				vBuffer[y1 + (7 - i) - 16][x1 + j - 8][0] = 0x20;//black
				vBuffer[y1 + (7 - i) - 16][x1 + j - 8][1] = 0x18;
				vBuffer[y1 + (7 - i) - 16][x1 + j - 8][2] = 0x08;
				break;
			case 2:
				vBuffer[y1 + (7 - i) - 16][x1 + j - 8][0] = 0x56;
				vBuffer[y1 + (7 - i) - 16][x1 + j - 8][1] = 0x68;
				vBuffer[y1 + (7 - i) - 16][x1 + j - 8][2] = 0x34;
				break;
			case 1:
				vBuffer[y1 + (7 - i) - 16][x1 + j - 8][0] = 0x70;
				vBuffer[y1 + (7 - i) - 16][x1 + j - 8][1] = 0xC0;
				vBuffer[y1 + (7 - i) - 16][x1 + j - 8][2] = 0x88;
				break;
			case 0:
				break; //transparent
			}
		}
	}
}

//Prints ALL tiles and sprites to screen
void printMemory(){
	static clock_t t2 = clock(); //update this every 1/4 sec

	if ((clock() - t2) >= 250)
	{
		t2 = clock();
		printVTable();
		printSTable();
		SDL_RenderPresent(rend);
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
				case 0: SDL_SetRenderDrawColor(rend, 0xFF, 0xFF, 0xFF, 0xFF); break;
				}
				SDL_RenderDrawPoint(rend, 16 * 8 + j + x * 8, i + y * 8);
			}
		}
	}
}


