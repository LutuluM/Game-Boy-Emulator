#pragma once
#include "defs.h"


/*
General Memory Map
0000-3FFF   16KB ROM Bank 00     (in cartridge, fixed at bank 00)
4000-7FFF   16KB ROM Bank 01..NN (in cartridge, switchable bank number)
8000-9FFF   8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)
A000-BFFF   8KB External RAM     (in cartridge, switchable bank, if any)
C000-CFFF   4KB Work RAM Bank 0 (WRAM)
D000-DFFF   4KB Work RAM Bank 1 (WRAM)  (switchable bank 1-7 in CGB Mode)
E000-FDFF   Same as C000-DDFF (ECHO)    (typically not used)
FE00-FE9F   Sprite Attribute Table (OAM)
FEA0-FEFF   Not Usable
FF00-FF7F   I/O Ports
FF80-FFFE   High RAM (HRAM)
FFFF        Interrupt Enable Register
*/
#ifndef cpuh
#define cpuh
typedef struct {
	uchar A;
	uchar B;
	uchar C;
	uchar D;
	uchar E;
	uchar H;
	uchar L;
	uchar Flags;
	ushort SP;
	ushort PC;
	uchar IME;
	uchar halted;
	unsigned int CPUTicks;
}GBCPU;

void writeMem(ushort location, ushort data, int bytes);
uchar readMem(ushort location);
void dumpMem(ushort start, ushort bytes);
void setJoy(uchar A);
void setLCDC(uchar A);
ushort getPC();
void setPC(ushort A);
uchar getIME();
void setIME(uchar A);


void OPSelect();
void extendedOPSelect();

void initCPU();///reset CPU and map Memory
uchar * loadROM(char *);
void changeRomLower(ushort location);

ushort adr16();///get 16 address from code next op codes
void pushPC();
void popPC();
void pushReg(ushort A);
void AND(uchar B);
void OR(uchar B);
void XOR(uchar B);
void CPL();
void SCF();
void CCF();
//
void ADD(uchar B);
void SUB(uchar B);
void ADC(uchar B);
void SBC(uchar B);
void ADDHL(ushort B);
void INC(uchar * A);
void DEC(uchar * A);
void INC2(uchar * A, uchar * B);
void DEC2(uchar * A, uchar * B);
void INCHL();
void DECHL();
void CP(uchar B);
//Rotate Left
void RL(uchar * A);
void RLC(uchar * A);
void RLA();
void RLCA();
void RR(uchar * A);
void RRC(uchar * A);
void RRA();
void RRCA();
//Shifts
void SLA(uchar * A);
void SRA(uchar * A);
void SRL(uchar * A);
//Bit testing/re-setting/swapping
void BIT(uchar A, uchar B);
void SWAP(uchar * A);
void RES(uchar *A, uchar B);
void SET(uchar *A, uchar B);
void DAA();
#endif