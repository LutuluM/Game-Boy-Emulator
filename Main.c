#include <SDL.h>
#include "defs.h"
#include "CPU.h"
#include "Graphics.h"
#include "IOs.h"

extern uchar * Brom;
extern uchar * GameCode;
extern uchar debug;
extern uchar running;


int main(int argc, char * argv[])
{
	char bios[] = "Enter Bios Name";
	char game[] = "Enter Game Name";
	
	Brom = loadROM(bios);
	GameCode = loadROM(game);
	initCPU();
	initGraphics();
	printf("Starting Emulator\n");

	running = 1;

	while (running)
	{
		
		OPSelect();		//runs next operation
		NextGraphic();	//prints next frame/line
		checkIO();		//checks for interrupts and processes IOs tasks

		printMemory(); //prints all of Vram to another window
	}

	exitGraphics();
	
	return 0;
}