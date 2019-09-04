#define _CRT_SECURE_NO_DEPRECATE
#include "stdio.h"
#include "stdlib.h"

#ifndef defsh
#define defsh

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

#define OP readMem(CPU.PC++)
#define FlagZ ((CPU.Flags & 0x80)>>7)
#define FlagN ((CPU.Flags & 0x40)>>6)
#define FlagH ((CPU.Flags & 0x20)>>5)
#define FlagC ((CPU.Flags & 0x10)>>4)
#define Flag CPU.Flags

#define HL (CPU.H << 8 | CPU.L)
#define DE (CPU.D << 8 | CPU.E)
#define BC (CPU.B << 8 | CPU.C)
#define AF (CPU.A << 8 | (CPU.Flags & 0xF0))


#define Vy 154
#define Vx 166
#define Winh 144
#define Winw 160
#define xoff readMem(0xFF43)
#define yoff readMem(0xFF42)

#endif
