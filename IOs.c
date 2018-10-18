#include "defs.h"
#include "IOs.h"
#include "CPU.h"
#include <SDL.h>


extern uchar sl;
extern uchar debug;
extern GBCPU CPU;
uchar running;

void checkIO() {
	keyboard();
	LCDC();
	Timer();
	if (getIME() == 1)
		Interrupts();
}

void keyboard(){
	SDL_Event event;
	static uchar buttons; //upper buttons lower d-pad
	uchar j5, j4,current,i;
	current = readMem(0xFF00);
	j5 = (current & 0x20) >> 5;
	j4 = (current & 0x10) >> 4;
	
	while(SDL_PollEvent(&event) != 0)//Check Keys
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
					dumpMem(0xFE00,0x9F);
					break;
				case SDLK_e://Debug toggle
					debug ^= 1;
					break;
				case SDLK_r:
					setPC(0x100);
					break;
				key:
					writeMem(0xFF0F, readMem(0xFF0F) | 0x10,1);
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

	if ((inter & 0x01) == 0x1)//V-blank
	{
		pushPC();
		setPC(0x40);
		setIME(0);
		writeMem(0xFF0F, inter & ~0x01,1); //clear flags manually
		CPU.halted = 0;
		//printf("V-blank\n");
	}
	else if ((inter & 0x02) == 0x2)//LCDC Status
	{
		pushPC();
		setPC(0x48);
		setIME(0);
		writeMem(0xFF0F, inter & ~0x02,1); //clear flags manually
		CPU.halted = 0;
		printf("LCDL \n");
	}
	else if ((inter & 0x04) == 0x4)//Timer
	{
		pushPC();
		setPC(0x50);
		setIME(0);
		writeMem(0xFF0F, inter & ~0x04,1); //clear flags manually
		CPU.halted = 0;
		//printf("Timer \n");
	}
	else if ((inter & 0x08)== 0x8)//Serial
	{
		pushPC();
		setPC(0x58);
		setIME(0);
		writeMem(0xFF0F, inter & ~0x08,1); //clear flags manually
		CPU.halted = 0;
		printf("Serial \n");
	}
	else if ((inter & 0x10) == 0x10)//Hi-Lo P10-P13 (buttons)
	{
		pushPC();
		setPC(0x60);
		setIME(0); 
		CPU.halted = 0;
		writeMem(0xFF0F, inter & ~0x10,1); //clear flags manually
		printf("Buttons \n");
	}
}

void Timer() {
	//Each instruction is 1-3 ticks
	static unsigned int clockDIV = 0;
	static unsigned int clockTIMER = 0;
	static uchar DIV = 0;
	static ushort TIMER = 0;
	ushort tTotal;
	ushort speed;

	if (DIV != readMem(0xFF04)) //check for write
	{
		DIV = 0;
		writeMem(0xFF04, DIV,1);
	}

	if (clockDIV > CPU.CPUTicks)
		tTotal = clockDIV - CPU.CPUTicks;
	else
		tTotal = CPU.CPUTicks - clockDIV;

	if (tTotal >= 1 << 6)
	{
		DIV++;
		writeMem(0xFF04, DIV,1);
	}

	if (readMem(0xFF07) & 0x4) //Timer is Enabled
	{ 
		switch (readMem(0xFF07) & 0x3)
		{
		case 0:
			speed = 1 << 8;
			break;
		case 1:
			speed = 1 << 2;
			break;
		case 2:
			speed = 1 << 4;
			break;
		case 3:
			speed = 1 << 6;
			break;
		}

		if (clockTIMER > CPU.CPUTicks)
			tTotal = clockTIMER - CPU.CPUTicks;
		else
			tTotal = CPU.CPUTicks - clockTIMER;

		if (tTotal >= speed)
		{
			TIMER++;
			clockTIMER = CPU.CPUTicks;
			writeMem(0xFF05, TIMER,1);
		}

		if (TIMER > 0xFF)
		{
			TIMER = readMem(0xFF06); //reset value
			writeMem(0xFF0F, readMem(0xFF0F) | 0x4,1);//Interrupt Flag set
		}
	}
}

void LCDC(){
	uchar inttable = readMem(0xFF41);
	//coincident flag
	if (readMem(0xFF45) == sl)
		inttable |= 0x4;
	else
		inttable &= ~0x4;
	setLCDC(inttable);
	//check for interrupts
	if ((inttable & 0x64) == 0x64)
	{
		writeMem(0xFF0F, readMem(0xFF0F) | 0x2,1);
	}
	if ((inttable & 0x10) == 0x10)
	{
		writeMem(0xFF0F, readMem(0xFF0F) | 0x2,1);
	}	
	//lowest 2 are for driver position
}

void DMA(ushort adr) {
	static unsigned int transferTime = CPU.CPUTicks;
	unsigned int elapsedTime;
	ushort i;
	if (transferTime > CPU.CPUTicks)
		elapsedTime = transferTime - CPU.CPUTicks;
	else
		elapsedTime = CPU.CPUTicks - transferTime;

	if (elapsedTime < 0xC8)//cycles to process transfer
		return;

	adr = adr << 8; //tranfer sprites from cartrage to workram
	for (i = 0; i<0x9F; i++)
	{
		writeMem(0xFE00 + i, readMem(adr + i),1);
	}
	//printf("TRANSFER COMPLETE \n");
}
