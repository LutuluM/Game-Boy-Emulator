#include <sdl.h>
#include "SDL_thread.h"
#include "defs.h"
#include "CPU.h"
#include "Memory.h"
#include "Graphics.h"
#include "IOs.h"
#include "sound.h"


extern uchar debug;
extern uchar running;

int main(int argc, char * argv[])
{
	printf("Initalizing System\n");
	char bios[] = "bios.bin";
	char game[] = "games\\red.gb";
	//char game[] = "test\\instr\\03.gb";
	//char game[] = "test\\00-cpu_test.gb";
	//char game[] = "test\\mooneye\\emulator-only\\mbc1\\boot_regs-A.gb";
	//char game[] = "test\\mooneye\\acceptance\\interrupts\\ie_push.gb";
	//"Working" drmario,snake,ds,battle,space

	loadBIOS(bios);
	loadGAME(game);

	initCPU();
	initMem();
	initGraphics();
	//initSound();
	printf("Starting Emulator\n");

	//SDL_Thread *debubThread;

	//debubThread = SDL_CreateThread((SDL_ThreadFunction)keyboard, "Debug", 0);
	//debubThread = SDL_CreateThread((SDL_ThreadFunction)initSound, "Debug", 0);
	//debubThread = SDL_CreateThread((SDL_ThreadFunction)printMemory, "Debug", 0);
	
	running = 1;

	while (running)
	{
		OPSelect();
		NextGraphic();
		checkIO();
		//printMemory();
	}

	saveGame();
	exitMem();
	exitGraphics();
	return 0;
}