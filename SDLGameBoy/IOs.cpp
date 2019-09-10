#include "defs.h"
#include "IOs.h"
#include "CPU.h"
#include "Memory.h"
#include "Graphics.h"
#include <SDL.h>

extern uchar debug;

uchar DivReset;
uchar running;

static uchar buttons; //upper buttons lower d-pad
uchar fpshelp;

void checkIO() {
	keyboard();
	LCDC();
	Timer();		
	if ((getIME() == 1) || DIHalt())
		Interrupts();
}

void keyboard() {
	SDL_Event event;
	uchar j5, j4, current, i;
	
	current = readMem(0xFF00);
	j5 = (current & 0x20) >> 5;
	j4 = (current & 0x10) >> 4;

	fpshelp = (fpshelp + 1) % 10000;//limit key polling to 100 cpu opcodes for fps problems
	if(fpshelp == 0)
	while (SDL_PollEvent(&event) != 0)//Check Keys
	{
		if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_t://Start
				buttons |= 0x80;
				goto key;
				break;
			case SDLK_y://Select
				buttons |= 0x40;
				goto key;
				break;
			case SDLK_h://B
				buttons |= 0x20;
				goto key;
				break;
			case SDLK_u://A
				buttons |= 0x10;
				goto key;
				break;
			case SDLK_s://Down
				buttons |= 0x8;
				goto key;
				break;
			case SDLK_w://UP
				buttons |= 0x4;
				goto key;
				break;
			case SDLK_a://Left
				buttons |= 0x2;
				goto key;
				break;
			case SDLK_d://Right
				buttons |= 0x1;
				goto key;
				break;
			case SDLK_ESCAPE://quit 
				running = 0;
			case SDLK_q://Memdump
				//dumpMem(0xFF00, 0xFE);
				dumpMem(0xFE00, 0xA0);
				break;
			case SDLK_e://Debug toggle
				debug ^= 1;
				break;
			case SDLK_r:
				setPC(0x100);
				break;
			key:
				writeMem(0xFF0F, readMem(0xFF0F) | 0x10);
			default:
				break;
			}
		}
		else if (event.type == SDL_KEYUP)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_t://Start
				buttons &= ~0x80;
				goto key;
				break;
			case SDLK_y://Select
				buttons &= ~0x40;
				goto key;
				break;
			case SDLK_h://B
				buttons &= ~0x20;
				goto key;
				break;
			case SDLK_u://A
				buttons &= ~0x10;
				goto key;
				break;
			case SDLK_s://Down
				buttons &= ~0x8;
				goto key;
				break;
			case SDLK_w://UP
				buttons &= ~0x4;
				goto key;
				break;
			case SDLK_a://Left
				buttons &= ~0x2;
				goto key;
				break;
			case SDLK_d://Right
				buttons &= ~0x1;
				goto key;
				break;
			default:
				break;
			}
		}
	}

	if (j5 == 0)//set the register to buttons
	{
		for (i = 4; i < 8; i++)
		{
			if (((buttons >> i) & 0x1) == 0x1)
				current &= ~(1 << (i - 4));
			else
				current |= (1 << (i - 4));
		}
	}
	else if (j4 == 0)//set register to d-pad
	{
		for (i = 0; i < 4; i++)
		{
			if (((buttons >> i) & 0x1) == 0x1)
				current &= ~(1 << (i));
			else
				current |= (1 << (i));
		}
	}
	setJoy(current);
}

void Interrupts(){
	uchar inter = readMem(0xFF0F); //where interrupts flags are
	inter &= readMem(0xFFFF);//only set one that have flags and are enabled

	if (DIHalt() && (inter > 0))//if it is halted and interrupts are disabled. it will unhalt when an interrupt occurs
	{
		halted(0);
	}
	else if ((inter & 0x01) == 0x1)//V-blank
	{
		pushPC();
		setPC(0x40);
		setIME(0);
		writeMem(0xFF0F, inter & ~0x01); //clear flags manually
		halted(0);
		//printf("V-blank\n");
	}
	else if ((inter & 0x02) == 0x2)//LCDC Status
	{
		pushPC();
		setPC(0x48);
		setIME(0);
		writeMem(0xFF0F, inter & ~0x02); //clear flags manually
		halted(0);
		//printf("LCDL \n");
	}
	else if ((inter & 0x04) == 0x4)//Timer
	{
		pushPC();
		setPC(0x50);
		setIME(0);
		writeMem(0xFF0F, inter & ~0x04); //clear flags manually
		halted(0);
		//printf("Timer \n");
	}
	else if ((inter & 0x08)== 0x8)//Serial
	{
		pushPC();
		setPC(0x58);
		setIME(0);
		writeMem(0xFF0F, inter & ~0x08); //clear flags manually
		halted(0);
		printf("Serial \n");
	}
	else if ((inter & 0x10) == 0x10)//Hi-Lo P10-P13 (buttons)
	{
		pushPC();
		setPC(0x60);
		setIME(0); 
		halted(0);
		writeMem(0xFF0F, inter & ~0x10); //clear flags manually
		printf("Buttons \n");
	}
}

void Timer() {//Fix this shit
	static ulong pTime = 0;
	static ushort DivCount = 0;
	static ushort DivCountOld = 0;
	ushort Time;
	uchar FallingEdge;

	if (DivReset)
	{
		DivReset = 0;
		DivCount = readMem(0xFF04);
	}

	while ((getCPUTicks()^pTime) != 0x0)//runs until caught up
	{
		DivCount += 0x4;
		if (readMem(0xFF07) & 0x4)//Checks if the clock is enabled
		{
			switch (readMem(0xFF07) & 0x3)//Reads the Timer Speed Value
			{
				case 0:
					if (((DivCount & 0x200) == 0) && ((DivCountOld & 0x200) == 0x200))
						FallingEdge = 1;
					else
						FallingEdge = 0;
					break;
				case 1:
					if (((DivCount & 0x08) == 0) && ((DivCountOld & 0x08) == 0x08))
						FallingEdge = 1;
					else
						FallingEdge = 0;
					break;
				case 2:
					if (((DivCount & 0x20) == 0) && ((DivCountOld & 0x20) == 0x20))
						FallingEdge = 1;
					else
						FallingEdge = 0;
					break;
				case 3:
					if (((DivCount & 0x80) == 0) && ((DivCountOld & 0x80) == 0x80))
						FallingEdge = 1;
					else
						FallingEdge = 0;
					break;
			}
			if (FallingEdge)
			{
				Time = readMem(0xFF05) + 1;//increment Current TIme
				writeMem(0xFF05, Time&0xFF);//write back to reg
				if (Time > 0xFF)
				{
					writeMem(0xFF05, readMem(0xFF06)); //Load FF05 with FF06 reset value
					writeMem(0xFF0F, readMem(0xFF0F) | 0x4);//Interrupt Flag set
				}
			}
		}
		pTime = pTime + 0x4;
		DivCountOld = DivCount;//save the divider time
		setDivTimer(DivCount >> 8);//Update Div Reg
	}
	if (getPC() == 0x100)
	{
		printf("Reg A is 0x%.4X\n", getReg(0));
		printf("current passedTime is %X\n", pTime);
		printf("current DivTotal is %X\n", DivCount);
		printf("current Div is %X\n", readMem(0xFF04));
		printf("current CPUTick is %X\n", getCPUTicks());
		dumpMem(0xFF10, 5);
	}
	//if (getPC() > 0x63) printf("SL is %X Video is %X\n", getSL(),readMem(0xFF41));
	/*if (getPC() == 0x6D || getPC() == 0x8b)
	{
		printf("current Div is %X SL is %X\n", readMem(0xFF04),getSL());
		printf("current DivTotal is %X\n", DivCount);
		getchar();
		
	}*/
	//if (getPC() == 0x6A)getchar();
}

void LCDC(){
	uchar inttable = readMem(0xFF41);
	//First 3 bits are read only
	//coincident flag
	if (readMem(0xFF45) == getSL())
		inttable |= 0x4;
	else
		inttable &= ~0x4;
	setLCDC(inttable);	//update LCDC readonly register

	//check for interrupts
	if ((inttable & 0x44) == 0x44)//coincidence
		writeMem(0xFF0F, readMem(0xFF0F) | 0x2);
	else if ((inttable & 0x23) == 0x22)//Mode 2 OAM 
		writeMem(0xFF0F, readMem(0xFF0F) | 0x2);
	else if ((inttable & 0x13) == 0x11)//Mode 1 V-Blank
		writeMem(0xFF0F, readMem(0xFF0F) | 0x2);
	else if ((inttable & 0x0B) == 0x08)//Mode 0 H-Blank
		writeMem(0xFF0F, readMem(0xFF0F) | 0x2);
	//lowest 2 are for videomode
	//bit-7 isn't used
}

void DMA(ushort adr) {
	ushort i;//just do it all at once

	adr = adr << 8; //tranfer sprites from cartrage to workram
	for (i = 0; i<0xA0; i++)
	{
		writeMem(0xFE00 + i, readMem(adr + i));
	}
	//printf("Transfer complete!\n");
}

void divReset() {
	DivReset = 1;
}