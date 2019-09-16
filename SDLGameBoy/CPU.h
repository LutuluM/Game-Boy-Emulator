#include "defs.h"

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
	uchar halt;
	volatile ulong CPUTicks;
}GBCPU;

extern uchar debug;


ushort getPC();
ushort getSP();
void setPC(ushort A);
uchar getIME();
void setIME(uchar A);
ulong getCPUTicks();
void halted(uchar A);
uchar DIHalt();
ushort getReg(uchar A);


void OPSelect();
void extendedOPSelect();

void initCPU();///reset CPU and map Memory

ushort adr16();///get 16 address from code next op codes
void pushPC();
void popPC();
void pushReg(ushort A);
//Logic Operations
void AND(uchar B);
void OR(uchar B);
void XOR(uchar B);
void CPL();
void SCF();
void CCF();
//Arithmetic Operations
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
//other
void ADDSP(uchar B);
void LDHLSP(uchar B);

#endif