#include <SDL2/SDL.h>
#include "defs.h"
#include "CPU.h"
#include "Memory.h"
#include "Graphics.h"
#include "IOs.h"
#include "Sound.hpp"
#include "TimeSync.hpp"


int main(int argc, char * argv[]){

    printf("Initalizing System\n");
    char bios[] = "bios.bin";
    char game[] = "games\\red.gb";
    //char game[] = "test\\instr\\02.gb";
    //char game[] = "test\\00-cpu_test.gb";
    //char game[] = "test\\mooneye\\emulator-only\\mbc1\\rom_16Mb.gb";
    //char game[] = "test\\mooneye\\acceptance\\interrupts\\ie_push.gb";
    //"Working" red,blue,tetris
    //ds,battle, needed double sprite
    //not working yellow

    
    initCPU();

    initGameMem();
    loadBIOS(bios);
    if (argc < 2)
        loadGAME(game);
    else
        loadGAME(argv[1]);
    initMem();
    
    initGraphics();
    initSound();
    printf("Starting Emulator\n");

    //SDL_Thread *debugThread;
    //debugThread = SDL_CreateThread((SDL_ThreadFunction)NextGraphic, "Debug", 0);
    //debugThread = SDL_CreateThread((SDL_ThreadFunction)initSound, "Debug", 0);
    //debugThread = SDL_CreateThread((SDL_ThreadFunction)printMemory, "Debug", 0);
    
    running = 1;
    //debug = 1;

    while (running)
    {
        OPSelect();
        NextGraphic();
        checkIO();
        updateSound();
        timeSync();
        //printMemory();
    }

    saveGame();
    exitMem();
    exitGraphics();
    return 0;
}