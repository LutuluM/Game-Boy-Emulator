#include "defs.h"

/*
General Memory Map
0000-3FFF   16KB ROM Bank 00     (in cartridge, fixed at bank 00)
4000-7FFF   16KB ROM Bank 01..NN (in cartridge, switchable bank number)
8000-9FFF   8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)
A000-BFFF   8KB External RAM     (in cartridge, switchable bank, if any)
C000-CFFF   4KB Work RAM Bank 0 (WRAM)
D000-DFFF   4KB Work RAM Bank 1 (WRAM)  (switchable bank 1-7 in CGB Mode)
E000-FDFF   Same as Work RAM Bank 0 (ECHO)    (typically not used)
FE00-FE9F   Sprite Attribute Table (OAM)
FEA0-FEFF   Not Usable
FF00-FF7F   I/O Ports
FF80-FFFE   High RAM (HRAM)
FFFF        Interrupt Enable Register
*/

#ifndef Memory
#define Memory

void writeMem(ushort location, uchar data);
void MBC1(ushort location, uchar data);
void MBC2(ushort location, uchar data);
void MBC3(ushort location, uchar data);
uchar readMem(ushort location);
void dumpMem(ushort start, ushort bytes);
void changeROM1(uchar data);
void changeRAM1(uchar data);
void changeROM3(uchar data);
void changeRAM3(uchar data);
void initMem();
void setJoy(uchar A);
void setLCDC(uchar A);
void setDivTimer(uchar A);
uchar * loadROM(char *);
void loadBIOS(char *);
void loadGAME(char *);
void loadSave(char *);
void saveGame();
void exitMem();
#endif