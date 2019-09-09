#include "CPU.h"
#include "IOs.h"
#include "Memory.h"


uchar debug;

///CPU
GBCPU CPU;
uchar HLm;

void initCPU() {
	CPU.A = 0;
	CPU.B = 0;
	CPU.C = 0;
	CPU.E = 0;
	Flag = 0;
	CPU.H = 0;
	CPU.L = 0;
	CPU.PC = 0;
	CPU.SP = 0;

	CPU.CPUTicks = 0;
}

void OPSelect()
{
	///A is the accumulator so its use is implicitly
	ushort adr; //used for 16-bit addresses
	uchar op;   //used for 8-bit values
	//if (debug) getchar();
	if (debug == 1)printf("PC:%.4X OP:%.2X ", CPU.PC, readMem(CPU.PC));
	
	if (CPU.halt) 
	{
		//printf("Halted\n");
		CPU.CPUTicks += 4;
		return;
	}

	op = readMem(CPU.PC++);
	switch (op)
	{
		case 0x00:  
			if (debug == 1)printf("NOP \n");
			CPU.CPUTicks += 4;
			break;
		case 0x01:
			CPU.C = OP;
			CPU.B = OP;
			if (debug == 1)printf("LD BC, %X \n", CPU.B << 8 | CPU.C);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x02:  
			adr = BC;
			if (debug == 1)printf("LD(BC)[%X],A \n", BC);
			writeMem(adr, CPU.A);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x03:  
			if (debug == 1)printf("INC BC \n");
			INC2(&CPU.B, &CPU.C);
			break;
		case 0x04: 
			if (debug == 1)printf("INC B \n");
			INC(&CPU.B);
			break;
		case 0x05:  
			if (debug == 1)printf("DEC B \n");
			DEC(&CPU.B);
			break;
		case 0x06:  
			op = OP;
			if (debug == 1)printf("LD B, op[%.2X] \n", op);
			CPU.B = op;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x07:  
			if (debug == 1)printf("RLCA \n");
			RLCA();
			break;
		case 0x08: 
			adr = adr16();
			if (debug == 1)printf("LD (adr)[%X],SP \n", adr);
			writeMem(adr++, CPU.SP & 0xFF);
			writeMem(adr, CPU.SP >> 8);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x09:  
			if (debug == 1)printf("ADD HL,BC \n");
			ADDHL(BC);
			break;
		case 0x0a:
			if (debug == 1)printf("LD A,(BC)[%X] \n", BC);
			CPU.A = readMem(BC);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x0b:
			if (debug == 1)printf("DEC BC \n");
			DEC2(&CPU.B,&CPU.C);
			break;
		case 0x0c: 
			if (debug == 1)printf("INC C\n");
			INC(&CPU.C);
			break;
		case 0x0d:  
			if (debug == 1)printf("DEC C \n");
			DEC(&CPU.C);
			break;
		case 0x0e: 
			op = OP;
			if (debug == 1)printf("LD C, op[%X] \n", op);
			CPU.C = op;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x0f:  
			if (debug == 1)printf("RRCA \n");
			RRCA();
			break;
		case 0x10:  
			printf("STOP 0\n");
			printf("PROGRAM COUNTER: %X", getPC());
			getchar();
			CPU.CPUTicks += 4;
			exit(0);
		case 0x11:  
			CPU.E = OP;
			CPU.D = OP;
			if (debug == 1)printf("LD DE,op2[%X] \n", DE);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x12:
			if (debug == 1)printf("LD (DE)[%X],A \n", DE);
			writeMem(DE, CPU.A);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x13:  
			if (debug == 1)printf("INC DE \n");
			INC2(&CPU.D, &CPU.E);
			break;
		case 0x14:  
			if (debug == 1)printf("INC D \n");
			INC(&CPU.D);
			break;
		case 0x15:  
			if (debug == 1)printf("DEC D \n");
			DEC(&CPU.D);
			break;
		case 0x16:  
			op = OP;
			if (debug == 1)printf("LD D, op[%X] \n", op);
			CPU.D = op;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x17:  
			if (debug == 1)printf("RLA \n");
			RLA();
			break;
		case 0x18:  
			op = OP;
			if (debug == 1)printf("JR op[%d] \n", (char)op);
			CPU.PC += (char)op;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x19:  
			if (debug == 1)printf("ADD HL,DE \n");
			ADDHL(DE);
			break;
		case 0x1a:  
			if (debug == 1)printf("LD A,(DE)[%X] \n", DE);
			CPU.A = readMem(DE);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x1b:  
			if (debug == 1)printf("DEC DE \n");
			DEC2(&CPU.D, &CPU.E);
			break;
		case 0x1c:  
			if (debug == 1)printf("INC E \n");
			INC(&CPU.E);
			break;
		case 0x1d:  
			if (debug == 1)printf("DEC E \n");
			DEC(&CPU.E);
			break;
		case 0x1e:  
			op = OP;
			if (debug == 1)printf("LD E, op[%X] \n", op);
			CPU.E = op;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x1f: 
			if (debug == 1)printf("RRA \n");
			RRA();
			break;
		case 0x20:  
			op = OP;
			if (debug == 1)printf("JR NZ, op[%d]\n", (char)op);
			if (FlagZ == 0)
			{
				CPU.CPUTicks += 4;
				CPU.PC += (char)op;
			}
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x21: 
			CPU.L = OP;
			CPU.H = OP;
			if (debug == 1)printf("LD HL,op2[%X] \n", HL);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x22:
			if (debug == 1)printf("LD (HL+)[%X++],A \n",HL);
			writeMem(HL, CPU.A);
			INC2(&CPU.H, &CPU.L);
			break;
		case 0x23:  
			if (debug == 1)printf("INC HL \n");
			INC2(&CPU.H, &CPU.L);
			break;
		case 0x24:  
			if (debug == 1)printf("INC H \n");
			INC(&CPU.H);
			break;
		case 0x25: 
			if (debug == 1)printf("DEC H \n");
			DEC(&CPU.H);
			break;
		case 0x26: 
			op = OP;
			if (debug == 1)printf("LD H, op[%X] \n", op);
			CPU.H = op;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x27:
			if(debug == 1)printf("DAA \n");
			DAA();
			break;
		case 0x28: 
			op = OP;
			if (debug == 1)printf("JR Z, op[%d] \n", (char)op);
			if (FlagZ == 1)
			{
				CPU.PC += (char)op;
				CPU.CPUTicks += 4;
			}
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x29:  
			if (debug == 1)printf("ADD HL,HL \n");
			ADDHL(HL);
			break;
		case 0x2a:
			if (debug == 1)printf("LD A, (HL+)[%X++] \n", HL);
			CPU.A = readMem(HL);
			INC2(&CPU.H, &CPU.L);
			break;
		case 0x2b:  
			if (debug == 1)printf("DEC HL \n");
			DEC2(&CPU.H, &CPU.L);
			break;
		case 0x2c:  
			if (debug == 1)printf("INC L \n");
			INC(&CPU.L);
			break;
		case 0x2d:  
			if (debug == 1)printf("DEC L \n");
			DEC(&CPU.L);
			break;
		case 0x2e:  
			op = OP;
			if (debug == 1)printf("LD L, op[%X] \n", op);
			CPU.L = op;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x2f:  
			if (debug == 1)printf("CPL \n");
			CPL();
			break;
		case 0x30:  
			op = OP;
			if (debug == 1)printf("JR NC, op[%d] \n", (char)op);
			if (FlagC == 0)
			{
				CPU.PC += (char)op;
				CPU.CPUTicks += 4;
			}
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x31:  
			adr = adr16();
			if (debug == 1)printf("LD SP, op2[%X] \n", adr);
			CPU.SP = adr;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x32:
			if (debug == 1)printf("LD (HL-)[%X--],A \n", HL);
			writeMem(HL, CPU.A);
			DEC2(&CPU.H, &CPU.L);
			break;
		case 0x33:
			if(debug == 1)printf("INC SP \n");
			CPU.SP++;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x34: 
			if(debug == 1)printf("INC (HL) \n");
			INCHL();
			break;
		case 0x35:  
			if(debug == 1)printf("DEC (HL) \n");
			DECHL();
			break;
		case 0x36:
			op = OP;
			if (debug == 1)printf("LD (HL)[%X], op[%X] \n", HL, op);
			writeMem(HL, op);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x37:  
			if (debug == 1)printf("SCF \n");
			SCF();
			break;
		case 0x38:  
			op = OP;
			if (debug == 1)printf("JR C, op[%d] \n", (char)op);
			if (FlagC == 1)
			{
				CPU.PC += (char)op;
				CPU.CPUTicks += 4;
			}
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x39:  
			if (debug == 1)printf("ADD HL,SP \n");
			ADDHL(CPU.SP);
			break;
		case 0x3a:
			if (debug == 1)printf("LD A, (HL-)[%X--] \n", HL);
			CPU.A = readMem(HL);
			DEC2(&CPU.H, &CPU.L);
			break;
		case 0x3b:
			if(debug == 1)printf("DEC SP \n");
			CPU.SP--;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x3c:  
			if (debug == 1)printf("INC A \n");
			INC(&CPU.A);
			break;
		case 0x3d:  
			if (debug == 1)printf("DEC A \n");
			DEC(&CPU.A);
			break;
		case 0x3e:  
			op = OP;
			if (debug == 1)printf("LD A, op[%X] \n", op);
			CPU.A = op;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x3f:  
			if (debug == 1)printf("CCF \n");
			CCF();
			break;
		case 0x40:  
			if (debug == 1)printf("LD B,B \n");
			CPU.B = CPU.B;
			CPU.CPUTicks += 4;
			break;
		case 0x41:  
			if (debug == 1)printf("LD B,C \n");
			CPU.B = CPU.C;
			CPU.CPUTicks += 4;
			break;
		case 0x42:  
			if (debug == 1)printf("LD B,D \n");
			CPU.B = CPU.D;
			CPU.CPUTicks += 4;
			break;
		case 0x43:  
			if (debug == 1)printf("LD B,E \n");
			CPU.B = CPU.E;
			CPU.CPUTicks += 4;
			break;
		case 0x44:  
			if (debug == 1)printf("LD B,H \n");
			CPU.B = CPU.H;
			CPU.CPUTicks += 4;
			break;
		case 0x45:  
			if (debug == 1)printf("LD B,L \n");
			CPU.B = CPU.L;
			CPU.CPUTicks += 4;
			break;
		case 0x46:
			if (debug == 1)printf("LD B,(HL)[%X] \n", HL);
			CPU.B = readMem(HL);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x47:  
			if (debug == 1)printf("LD B,A \n");
			CPU.B = CPU.A;
			CPU.CPUTicks += 4;
			break;
		case 0x48: 
			if (debug == 1)printf("LD C,B \n");
			CPU.C = CPU.B;
			CPU.CPUTicks += 4;
			break;
		case 0x49:  
			if (debug == 1)printf("LD C,C \n");
			CPU.C = CPU.C;
			CPU.CPUTicks += 4;
			break;
		case 0x4a:  
			if (debug == 1)printf("LD C,D \n");
			CPU.C = CPU.D;
			CPU.CPUTicks += 4;
			break;
		case 0x4b:  
			if (debug == 1)printf("LD C,E \n");
			CPU.C = CPU.E;
			CPU.CPUTicks += 4;
			break;
		case 0x4c:  
			if (debug == 1)printf("LD C,H \n");
			CPU.C = CPU.H;
			CPU.CPUTicks += 4;
			break;
		case 0x4d:  
			if (debug == 1)printf("LD C,L \n");
			CPU.C = CPU.L;
			CPU.CPUTicks += 4;
			break;
		case 0x4e:
			if (debug == 1)printf("LD C,(HL)[%X] \n", HL);
			CPU.C = readMem(HL);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x4f:  
			if (debug == 1)printf("LD C,A \n");
			CPU.C = CPU.A;
			CPU.CPUTicks += 4;
			break;
		case 0x50: 
			if (debug == 1)printf("LD D,B \n");
			CPU.D = CPU.B;
			CPU.CPUTicks += 4;
			break;
		case 0x51:  
			if (debug == 1)printf("LD D,C \n");
			CPU.D = CPU.C;
			CPU.CPUTicks += 4;
			break;
		case 0x52: 
			if (debug == 1)printf("LD D,D \n");
			CPU.D = CPU.D;
			CPU.CPUTicks += 4;
			break;
		case 0x53: 
			if (debug == 1)printf("LD D,E \n");
			CPU.D = CPU.E;
			CPU.CPUTicks += 4;
			break;
		case 0x54: 
			if (debug == 1)printf("LD D,H \n");
			CPU.D = CPU.H;
			CPU.CPUTicks += 4;
			break;
		case 0x55: 
			if (debug == 1)printf("LD D,L \n");
			CPU.D = CPU.L;
			CPU.CPUTicks += 4;
			break;
		case 0x56:
			if (debug == 1)printf("LD D,(HL)[%X] \n", HL);
			CPU.D = readMem(HL);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x57: 
			if (debug == 1)printf("LD D,A \n");
			CPU.D = CPU.A;
			CPU.CPUTicks += 4;
			break;
		case 0x58: 
			if (debug == 1)printf("LD E,B \n");
			CPU.E = CPU.B;
			CPU.CPUTicks += 4;
			break;
		case 0x59: 
			if (debug == 1)printf("LD E,C \n");
			CPU.E = CPU.C;
			CPU.CPUTicks += 4;
			break;
		case 0x5a: 
			if (debug == 1)printf("LD E,D \n");
			CPU.E = CPU.D;
			CPU.CPUTicks += 4;
			break;
		case 0x5b:  
			if (debug == 1)printf("LD E,E \n");
			CPU.E = CPU.E;
			CPU.CPUTicks += 4;
			break;
		case 0x5c:  
			if (debug == 1)printf("LD E,H \n");
			CPU.E = CPU.H;
			CPU.CPUTicks += 4;
			break;
		case 0x5d: 
			if (debug == 1)printf("LD E,L \n");
			CPU.E = CPU.L;
			CPU.CPUTicks += 4;
			break;
		case 0x5e:
			if (debug == 1)printf("LD E,(HL)[%X] \n", HL);
			CPU.E = readMem(HL);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x5f:  
			if (debug == 1)printf("LD E,A \n");
			CPU.E = CPU.A;
			CPU.CPUTicks += 4;
			break;
		case 0x60: 
			if (debug == 1)printf("LD H,B \n");
			CPU.H = CPU.B;
			CPU.CPUTicks += 4;
			break;
		case 0x61: 
			if (debug == 1)printf("LD H,C \n");
			CPU.H = CPU.C;
			CPU.CPUTicks += 4;
			break;
		case 0x62: 
			if (debug == 1)printf("LD H,D \n");
			CPU.H = CPU.D;
			CPU.CPUTicks += 4;
			break;
		case 0x63:  
			if (debug == 1)printf("LD H,E \n");
			CPU.H = CPU.E;
			CPU.CPUTicks += 4;
			break;
		case 0x64: 
			if (debug == 1)printf("LD H,H \n");
			CPU.H = CPU.H;
			CPU.CPUTicks += 4;
			break;
		case 0x65:  
			if (debug == 1)printf("LD H,L \n");
			CPU.H = CPU.L;
			CPU.CPUTicks += 4;
			break;
		case 0x66:
			if (debug == 1)printf("LD H,(HL)[%X] \n", HL);
			CPU.H = readMem(HL);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x67:  
			if (debug == 1)printf("LD H,A \n");
			CPU.H = CPU.A;
			CPU.CPUTicks += 4;
			break;
		case 0x68:  
			if (debug == 1)printf("LD L,B \n");
			CPU.L = CPU.B;
			CPU.CPUTicks += 4;
			break;
		case 0x69:  
			if (debug == 1)printf("LD L,C \n");
			CPU.L = CPU.C;
			CPU.CPUTicks += 4;
			break;
		case 0x6a:  
			if (debug == 1)printf("LD L,D \n");
			CPU.L = CPU.D;
			CPU.CPUTicks += 4;
			break;
		case 0x6b:  
			if (debug == 1)printf("LD L,E \n");
			CPU.L = CPU.E;
			CPU.CPUTicks += 4;
			break;
		case 0x6c:  
			if (debug == 1)printf("LD L,H \n");
			CPU.L = CPU.H;
			CPU.CPUTicks += 4;
			break;
		case 0x6d:  
			if (debug == 1)printf("LD L,L \n");
			CPU.L = CPU.L;
			CPU.CPUTicks += 4;
			break;
		case 0x6e:
			if (debug == 1)printf("LD L,(HL)[%X] \n", HL);
			CPU.L = readMem(HL);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x6f:  
			if (debug == 1)printf("LD L,A \n");
			CPU.L = CPU.A;
			CPU.CPUTicks += 4;
			break;
		case 0x70:
			if (debug == 1)printf("LD (HL)[%X],B \n", HL);
			writeMem(HL, CPU.B);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x71:
			if (debug == 1)printf("LD (HL)[%X],C \n", HL);
			writeMem(HL, CPU.C);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x72:
			if (debug == 1)printf("LD (HL)[%X],D \n", HL);
			writeMem(HL, CPU.D);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x73:
			if (debug == 1)printf("LD (HL)[%X],E \n", HL);
			writeMem(HL, CPU.E);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x74:
			if (debug == 1)printf("LD (HL)[%X],H \n", HL);
			writeMem(HL, CPU.H);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x75:
			if (debug == 1)printf("LD (HL)[%X],L \n", HL);
			writeMem(HL, CPU.L);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x76:  
			if (debug == 1)printf("HALT \n");
			halted(1);
			CPU.CPUTicks += 4;
			break;
		case 0x77:
			if (debug == 1)printf("LD (HL)[%X],A \n", HL);
			writeMem(HL, CPU.A);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x78:  
			if (debug == 1)printf("LD A,B \n");
			CPU.A = CPU.B;
			CPU.CPUTicks += 4;
			break;
		case 0x79:  
			if (debug == 1)printf("LD A,C \n");
			CPU.A = CPU.C;
			CPU.CPUTicks += 4;
			break;
		case 0x7a: 
			if (debug == 1)printf("LD A,D \n");
			CPU.A = CPU.D;
			CPU.CPUTicks += 4;
			break;
		case 0x7b: 
			if (debug == 1)printf("LD A,E \n");
			CPU.A = CPU.E;
			CPU.CPUTicks += 4;
			break;
		case 0x7c:  
			if (debug == 1)printf("LD A,H \n");
			CPU.A = CPU.H;
			CPU.CPUTicks += 4;
			break;
		case 0x7d:  
			if (debug == 1)printf("LD A,L \n");
			CPU.A = CPU.L;
			CPU.CPUTicks += 4;
			break;
		case 0x7e:
			if (debug == 1)printf("LD A,(HL)[%X] \n", HL);
			CPU.A = readMem(HL);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0x7f:  
			if (debug == 1)printf("LD A,A \n");
			CPU.A = CPU.A;
			CPU.CPUTicks += 4;
			break;
		case 0x80:  
			if (debug == 1)printf("ADD A, B \n");
			ADD(CPU.B);
			break;
		case 0x81: 
			if (debug == 1)printf("ADD A, C \n");
			ADD(CPU.C);
			break;
		case 0x82:  
			if (debug == 1)printf("ADD A, D \n");
			ADD(CPU.D);
			break;
		case 0x83:  
			if (debug == 1)printf("ADD A, E \n");
			ADD(CPU.E);
			break;
		case 0x84: 
			if (debug == 1)printf("ADD A, H \n");
			ADD(CPU.H);
			break;
		case 0x85: 
			if (debug == 1)printf("ADD A, L \n");
			ADD(CPU.L);
			break;
		case 0x86:
			if (debug == 1)printf("ADD A, (HL)\n");
			ADD(readMem(HL));
			CPU.CPUTicks += 4;
			break;
		case 0x87: 
			if (debug == 1)printf("ADD A, A \n");
			ADD(CPU.A);
			break;
		case 0x88: 
			if (debug == 1)printf("ADC A, B \n");
			ADC(CPU.B);
			break;
		case 0x89:  
			if (debug == 1)printf("ADC A, C \n");
			ADC(CPU.C);
			break;
		case 0x8A: 
			if (debug == 1)printf("ADC A, D \n");
			ADC(CPU.D);
			break;
		case 0x8B:  
			if (debug == 1)printf("ADC A, E \n");
			ADC(CPU.E);
			break;
		case 0x8C:  
			if (debug == 1)printf("ADC A, H \n");
			ADC(CPU.H);
			break;
		case 0x8D: 
			if (debug == 1)printf("ADC A, L \n");
			ADC(CPU.L);
			break;
		case 0x8E:
			if (debug == 1)printf("ADC A, (HL)\n");
			ADC(readMem(HL));
			CPU.CPUTicks += 4;
			break;
		case 0x8F:  
			if (debug == 1)printf("ADC A, A \n");
			ADC(CPU.A);
			break;
		case 0x90:  
			if (debug == 1)printf("SUB B \n");
			SUB(CPU.B);
			break;
		case 0x91:  
			if (debug == 1)printf("SUB C \n");
			SUB(CPU.C);
			break;
		case 0x92:  
			if (debug == 1)printf("SUB D \n");
			SUB(CPU.D);
			break;
		case 0x93:
			if (debug == 1)printf("SUB E \n");
			SUB(CPU.E);
			break;
		case 0x94: 
			if (debug == 1)printf("SUB H \n");
			SUB(CPU.H);
			break;
		case 0x95:
			if (debug == 1)printf("SUB L \n");
			SUB(CPU.L);
			break;
		case 0x96:
			if (debug == 1)printf("SUB (HL)\n");
			SUB(readMem(HL));
			CPU.CPUTicks += 4;
			break;
		case 0x97:
			if (debug == 1)printf("SUB A \n");
			SUB(CPU.A);
			break;
		case 0x98:
			if (debug == 1)printf("SBC B \n");
			SBC(CPU.B);
			break;
		case 0x99: 
			if (debug == 1)printf("SBC C \n");
			SBC(CPU.C);
			break;
		case 0x9a:  
			if (debug == 1)printf("SBC D \n");
			SBC(CPU.D);
			break;
		case 0x9b: 
			if (debug == 1)printf("SBC E \n");
			SBC(CPU.E);
			break;
		case 0x9c: 
			if (debug == 1)printf("SBC H \n");
			SBC(CPU.H);
			break;
		case 0x9d: 
			if (debug == 1)printf("SBC L \n");
			SBC(CPU.L);
			break;
		case 0x9e:
			if (debug == 1)printf("SBC (HL)\n");
			SBC(readMem(HL));
			CPU.CPUTicks += 4;
			break;
		case 0x9f: 
			if (debug == 1)printf("SBC A \n");
			SBC(CPU.A);
			break;
		case 0xa0: 
			if (debug == 1)printf("AND B \n");
			AND(CPU.B);
			break;
		case 0xa1: 
			if (debug == 1)printf("AND C \n");
			AND(CPU.C);
			break;
		case 0xa2: 
			if (debug == 1)printf("AND D \n");
			AND(CPU.D);
			break;
		case 0xa3: 
			if (debug == 1)printf("AND E \n");
			AND(CPU.E);
			break;
		case 0xa4: 
			if (debug == 1)printf("AND H \n");
			AND(CPU.H);
			break;
		case 0xa5: 
			if (debug == 1)printf("AND L \n");
			AND(CPU.L);
			break;
		case 0xa6:
			if (debug == 1)printf("AND (HL)\n");
			AND(readMem(HL));
			CPU.CPUTicks += 4;
			break;
		case 0xa7: 
			if (debug == 1)printf("AND A \n");
			AND(CPU.A);
			break;
		case 0xa8:  
			if (debug == 1)printf("XOR B \n");
			XOR(CPU.B);
			break;
		case 0xa9: 
			if (debug == 1)printf("XOR C \n");
			XOR(CPU.C);
			break;
		case 0xaa: 
			if (debug == 1)printf("XOR D \n");
			XOR(CPU.D);
			break;
		case 0xab: 
			if (debug == 1)printf("XOR E \n");
			XOR(CPU.E);
			break;
		case 0xac: 
			if (debug == 1)printf("XOR H \n");
			XOR(CPU.H);
			break;
		case 0xad:  
			if (debug == 1)printf("XOR L \n");
			XOR(CPU.L);
			break;
		case 0xae:
			if (debug == 1)printf("XOR (HL)\n");
			XOR(readMem(HL));
			CPU.CPUTicks += 4;
			break;
		case 0xaf: 
			if (debug == 1)printf("XOR A \n");
			XOR(CPU.A);
			break;
		case 0xb0: 
			if (debug == 1)printf("OR B \n");
			OR(CPU.B);
			break;
		case 0xb1: 
			if (debug == 1)printf("OR C \n");
			OR(CPU.C);
			break;
		case 0xb2:  
			if (debug == 1)printf("OR D \n");
			OR(CPU.D);
			break;
		case 0xb3: 
			if (debug == 1)printf("OR E \n");
			OR(CPU.E);
			break;
		case 0xb4: 
			if (debug == 1)printf("OR H \n");
			OR(CPU.H);
			break;
		case 0xb5: 
			if (debug == 1)printf("OR L \n");
			OR(CPU.L);
			break;
		case 0xb6:
			if (debug == 1)printf("OR (HL)\n");
			OR(readMem(HL));
			CPU.CPUTicks += 4;
			break;
		case 0xb7: 
			if (debug == 1)printf("OR A \n");
			OR(CPU.A);
			break;
		case 0xb8:
			if (debug == 1)printf("CP B \n");
			CP(CPU.B);
			break;
		case 0xb9:
			if (debug == 1)printf("CP C \n");
			CP(CPU.C);
			break;
		case 0xba:
			if (debug == 1)printf("CP D \n");
			CP(CPU.D);
			break;
		case 0xbb: 
			if (debug == 1)printf("CP E \n");
			CP(CPU.E);
			break;
		case 0xbc: 
			if (debug == 1)printf("CP H \n");
			CP(CPU.H);
			break;
		case 0xbd:
			if (debug == 1)printf("CP L \n");
			CP(CPU.L);
			break;
		case 0xbe:
			if (debug == 1)printf("CP (HL)\n");
			CP(readMem(HL));
			CPU.CPUTicks += 4;
			break;
		case 0xbf:
			if (debug == 1)printf("CP A \n");
			CP(CPU.A);
			break;
		case 0xc0: 
			if (debug == 1)printf("RET NZ \n");
			if (FlagZ == 0)
				popPC();
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0xc1: 
			if (debug == 1)printf("POP BC \n");
			CPU.C = readMem(CPU.SP++);
			CPU.B = readMem(CPU.SP++);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0xc2: 
			adr = adr16();
			if (debug == 1)printf("JP NZ, op2[%X] \n", adr);
			if (FlagZ == 0)
			{
				CPU.PC = adr;
				CPU.CPUTicks += 4;
			}
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0xc3:
			adr = adr16();
			if (debug == 1)printf("JP op2[%X] \n", adr);
			CPU.PC = adr;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0xc4: 
			adr = adr16();
			if (debug == 1)printf("CALL NZ, op2[%X] \n", adr);
			if (FlagZ == 0)
			{
				pushPC();
				CPU.PC = adr;
			}
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0xc5: 
			if (debug == 1)printf("PUSH BC \n");
			pushReg(BC);
			break;
		case 0xc6: 
			op = OP;
			if (debug == 1)printf("ADD A, op[%X] \n", op);
			ADD(op);
			CPU.CPUTicks += 4;
			break;
		case 0xc7: 
			if (debug == 1)printf("RST 00H \n");
			pushPC();
			CPU.PC = 0;
			CPU.CPUTicks += 4;
			break;
		case 0xc8:  
			if (debug == 1)printf("RET Z \n");
			if (FlagZ == 1)
				popPC();
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0xc9:
			if (debug == 1)printf("RET \n");
			popPC();
			CPU.CPUTicks += 4;
			break;
		case 0xca:  
			adr = adr16();
			if (debug == 1)printf("JP Z, op2[%X] \n", adr);
			if (FlagZ == 1) 
			{
				CPU.PC = adr;
				CPU.CPUTicks += 4;
				
			}
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0xcb: 
			if (debug == 1)printf("Extended OP \n");
			CPU.CPUTicks += 4;
			extendedOPSelect();
			break;
		case 0xcc:
			adr = adr16();
			if (debug == 1)printf("CALL Z, op2[%X] \n", adr);
			if (FlagZ == 1)
			{
				pushPC();
				CPU.PC = adr;
			}
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0xcd:
			adr = adr16();
			if (debug == 1)printf("CALL op2[%X] \n", adr);
			pushPC();
			CPU.PC = adr;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0xce: 
			op = OP;
			if (debug == 1)printf("ADC A, op[%X] \n", op);
			ADC(op);
			CPU.CPUTicks += 4;
			break;
		case 0xcf:
			if (debug == 1)printf("RST 08H \n");
			pushPC();
			CPU.PC = 0x08;
			CPU.CPUTicks += 4;
			break;
		case 0xd0: 
			if (debug == 1)printf("RET NC \n");
			if (FlagC == 0)
				popPC();
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0xd1: 
			if (debug == 1)printf("POP DE \n");
			CPU.E = readMem(CPU.SP++);
			CPU.D = readMem(CPU.SP++);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0xd2: 
			adr = adr16();
			if (debug == 1)printf("JP NC, op2[%X] \n", adr);
			if (FlagC == 0)
			{
				CPU.PC = adr;
				CPU.CPUTicks += 4;
			}
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
			//case 0xd3://if(debug == 1)printf(" UNDEFINED \n");break;
		case 0xd4: 
			adr = adr16();
			if (debug == 1)printf("CALL NC, op2[%X] \n", adr);
			if (FlagC == 0)
			{
				pushPC();
				CPU.PC = adr;
			}
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0xd5:  
			if (debug == 1)printf("PUSH DE \n");
			pushReg(DE);
			break;
		case 0xd6:  
			op = OP;
			if (debug == 1)printf("SUB, op[%X] \n", op);
			SUB(op);
			CPU.CPUTicks += 4;
			break;
		case 0xd7:  
			if (debug == 1)printf("RST 10H \n");
			pushPC();
			CPU.PC = 0x10;
			CPU.CPUTicks += 4;
			break;
		case 0xd8: 
			if (debug == 1)printf("RET C \n");
			if (FlagC == 1)
				popPC();
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0xd9:  
			if (debug == 1)printf("RETI \n");
			popPC();
			CPU.IME = 1;
			CPU.CPUTicks += 4;
			OPSelect();
			break;
		case 0xda:  
			adr = adr16();
			if (debug == 1)printf("JP C, op2[%X] \n", adr);
			if (FlagC == 1) 
			{
				CPU.PC = adr;
				CPU.CPUTicks += 4;
			}
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
			//case 0xdb://if(debug == 1)printf(" UNDEFINED \n");break;
		case 0xdc:  
			adr = adr16();
			if (debug == 1)printf("CALL C, op2[%X] \n", adr);
			if (FlagC == 1)
			{
				pushPC();
				CPU.PC = adr;
			}
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
			//case 0xdd://if(debug == 1)printf(" UNDEFINED \n");break;
		case 0xde:  
			op = OP;
			if (debug == 1)printf("SBC A, op[%X] \n", op);
			SBC(op);
			CPU.CPUTicks += 4;
			break;
		case 0xdf: 
			if (debug == 1)printf("RST 18H \n");
			pushPC();
			CPU.PC = 0x18;
			CPU.CPUTicks += 4;
			break;
		case 0xe0: 
			adr = (0xFF00 | OP);
			if (debug == 1)printf("LDH (0xFF00+op)[%X],A \n", adr);
			writeMem(adr, CPU.A);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0xe1:
			if (debug == 1)printf("POP HL \n");
			CPU.L = readMem(CPU.SP++);
			CPU.H = readMem(CPU.SP++);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0xe2: 
			adr = (0xFF00 | CPU.C);
			if (debug == 1)printf("LD (0xFF00 + CPU.C)[%X],A \n", adr);
			writeMem(adr, CPU.A);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
			//case 0xe3://if(debug == 1)printf(" UNDEFINED \n");break;
			//case 0xe4://if(debug == 1)printf(" UNDEFINED \n");break;
		case 0xe5: 
			if (debug == 1)printf("PUSH HL \n");
			pushReg(HL);
			break;
		case 0xe6: 
			op = OP;
			if (debug == 1)printf("AND op[%X] \n", op);
			AND(op);
			CPU.CPUTicks += 4;
			break;
		case 0xe7: 
			if (debug == 1)printf("RST 20H \n");
			pushPC();
			CPU.PC = 0x20;
			CPU.CPUTicks += 4;
			break;
		case 0xe8:
			op = OP;
			if (debug == 1)printf("ADD SP, op[%d] \n", op);
			ADDSP(op);
			break;
		case 0xe9:
			if (debug == 1)printf("JP HL[%X] \n", HL);
			CPU.PC = HL;
			CPU.CPUTicks += 4;
			break;
		case 0xea: 
			adr = adr16();
			if (debug == 1)printf("LD (op2)[%X],A \n", adr);
			writeMem(adr, CPU.A);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
			//case 0xeb://if(debug == 1)printf(" UNDEFINED \n");break;
			//case 0xec://if(debug == 1)printf(" UNDEFINED \n");break;
			//case 0xed://if(debug == 1)printf(" UNDEFINED \n");break;
		case 0xee:  
			op = OP;
			if (debug == 1)printf("XOR op[%X] \n", op);
			XOR(op);
			CPU.CPUTicks += 4;
			break;
		case 0xef: 
			if (debug == 1)printf("RST 28H \n");
			pushPC();
			CPU.PC = 0x28;
			CPU.CPUTicks += 4;
			break;
		case 0xf0: 
			adr = (0xFF00 | OP);
			if (debug == 1)printf("LDH A, (op2)[%X] \n", adr);
			CPU.A = readMem(adr);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0xf1:
			if (debug == 1)printf("POP AF [%X]\n",CPU.PC);
			CPU.Flags = 0xF0 & readMem(CPU.SP++);		
			CPU.A = readMem(CPU.SP++);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0xf2: 
			adr = (0xFF00 | CPU.C);
			if (debug == 1)printf("LD A,(0xFF00+CPU.C)[%X] \n", adr);
			CPU.A = readMem(adr);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0xf3: 
			if (debug == 1)printf("DI \n");
			CPU.IME = 0;
			CPU.CPUTicks += 4;
			break;
			//case 0xf4://if(debug == 1)printf(" UNDEFINED \n");break;
		case 0xf5: 
			if (debug == 1)printf("PUSH AF \n");
			pushReg(AF);
			break;
		case 0xf6: 
			op = OP;
			if (debug == 1)printf("OR op[%X] \n", op);
			OR(op);
			CPU.CPUTicks += 4;
			break;
		case 0xf7: 
			if (debug == 1)printf("RST 30H \n");
			pushPC();
			CPU.PC = 0x30;
			CPU.CPUTicks += 4;
			break;
		case 0xf8:
			op = OP;
			if (debug == 1)printf("LD HL, SP + op[%X] \n", op);
			LDHLSP(op);
			break;
		case 0xf9:
			if (debug == 1)printf("LD SP,HL \n");
			CPU.SP = HL;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0xfa:
			adr = adr16();
			if (debug == 1)printf("LD A,(op2)[%X] \n", adr);
			CPU.A = readMem(adr);
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			CPU.CPUTicks += 4;
			break;
		case 0xfb:
			if (debug == 1)printf("EI \n");
			CPU.IME = 1;
			CPU.CPUTicks += 4;
			OPSelect(); //run next op before interrupt occurs
			break;
			//case 0xfc://if(debug == 1)printf(" UNDEFINED \n");break;
			//case 0xfd://if(debug == 1)printf(" UNDEFINED \n");break;
		case 0xfe: 
			op = OP;
			if (debug == 1)printf("CP op[%X] \n", op);
			CP(op);
			CPU.CPUTicks += 4;
			break;
		case 0xff:
			if (debug == 1)printf("RST 38H \n");
			pushPC();
			CPU.PC = 0x38;
			CPU.CPUTicks += 4;
			break;
		default:
			printf("Bad Opcode %X @ PC:%X\n", op, CPU.PC - 1);
			getchar();
			break;
	}
}

void extendedOPSelect() {
	uchar op;	//used for 8-bit values
	if (debug == 1)printf("PC:%.4X XP:%.2X ", CPU.PC, readMem(CPU.PC));

	op = readMem(CPU.PC++);
	switch (op)
	{
		case 0x00:
			if(debug == 1)printf("RLC B\n");
			RLC(&CPU.B);
			break;
		case 0x01:
			if (debug == 1)printf("RLC C\n");
			RLC(&CPU.C);
			break;
		case 0x02:
			if (debug == 1)printf("RLC D\n");
			RLC(&CPU.D);
			break;
		case 0x03:
			if (debug == 1)printf("RLC E\n");
			RLC(&CPU.E);
			break;
		case 0x04:
			if (debug == 1)printf("RLC H\n");
			RLC(&CPU.H);
			break;
		case 0x05:
			if (debug == 1)printf("RLC L\n");
			RLC(&CPU.L);
			break;
		case 0x06:
			if (debug == 1)printf("RLC (HL)\n");
			HLm = readMem(HL);
			RLC(&HLm);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0x07:
			if (debug == 1)printf("RLC A\n");
			RLC(&CPU.A);
			break;
		case 0x08:
			if (debug == 1)printf("RRC B\n");
			RRC(&CPU.B);
			break;
		case 0x09:
			if (debug == 1)printf("RRC C\n");
			RRC(&CPU.C);
			break;
		case 0x0a:
			if (debug == 1)printf("RRC D\n");
			RRC(&CPU.D);
			break;
		case 0x0b:
			if (debug == 1)printf("RRC E\n");
			RRC(&CPU.E);
			break;
		case 0x0c:
			if (debug == 1)printf("RRC H\n");
			RRC(&CPU.H);
			break;
		case 0x0d:
			if (debug == 1)printf("RRC L\n");
			RRC(&CPU.L);
			break;
		case 0x0e:
			if (debug == 1)printf("RRC (HL)\n");
			HLm = readMem(HL);
			RRC(&HLm);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0x0f:
			if (debug == 1)printf("RRC A\n");
			RRC(&CPU.A);
			break;
		case 0x10:
			if (debug == 1)printf("RL B\n");
			RL(&CPU.B);
			break;
		case 0x11:  
			if (debug == 1)printf("RL C\n");
			RL(&CPU.C);
			break;
		case 0x12:
			if (debug == 1)printf("RL D\n");
			RL(&CPU.D);
			break;
		case 0x13:
			if (debug == 1)printf("RL E\n");
			RL(&CPU.E);
			break;
		case 0x14:
			if (debug == 1)printf("RL H\n");
			RL(&CPU.H);
			break;
		case 0x15:
			if (debug == 1)printf("RL L\n");
			RL(&CPU.L);
			break;
		case 0x16:
			if (debug == 1)printf("RL (HL)\n");
			HLm = readMem(HL);
			RL(&HLm);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0x17:
			if (debug == 1)printf("RL A\n");
			RL(&CPU.A);
			break;
		case 0x18:
			if (debug == 1)printf("RR B \n");
			RR(&CPU.B);
			break;
		case 0x19:
			if (debug == 1)printf("RR C \n");
			RR(&CPU.C);
			break;
		case 0x1a:  
			if (debug == 1)printf("RR D \n");
			RR(&CPU.D);
			break;
		case 0x1b:
			if (debug == 1)printf("RR E \n");
			RR(&CPU.E);
			break;
		case 0x1c:
			if (debug == 1)printf("RR H \n");
			RR(&CPU.H);
			break;
		case 0x1d:
			if (debug == 1)printf("RR L \n");
			RR(&CPU.L);
			break;
		case 0x1e:
			if (debug == 1)printf("RR (HL)\n");
			HLm = readMem(HL);
			RR(&HLm);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0x1f:
			if (debug == 1)printf("RR A \n");
			RR(&CPU.A);
			break;
		case 0x20:
			if (debug == 1)printf("SLA B\n");
			SLA(&CPU.B);
			break;
		case 0x21:
			if (debug == 1)printf("SLA C\n");
			SLA(&CPU.C);
			break;
		case 0x22:
			if (debug == 1)printf("SLA D\n");
			SLA(&CPU.D);
			break;
		case 0x23:
			if (debug == 1)printf("SLA E\n");
			SLA(&CPU.E);
			break;
		case 0x24:
			if (debug == 1)printf("SLA H\n");
			SLA(&CPU.H);
			break;
		case 0x25:
			if (debug == 1)printf("SLA L\n");
			SLA(&CPU.L);
			break;
		case 0x26:
			if (debug == 1)printf("SLA (HL)\n");
			HLm = readMem(HL);
			SLA(&HLm);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0x27:
			if (debug == 1)printf("SLA A\n");
			SLA(&CPU.A);
			break;
		case 0x28:
			if (debug == 1)printf("SRA B\n");
			SRA(&CPU.B);
			break;
		case 0x29:
			if (debug == 1)printf("SRA C\n");
			SRA(&CPU.C);
			break;
		case 0x2a:
			if (debug == 1)printf("SRA D\n");
			SRA(&CPU.D);
			break;
		case 0x2b:
			if (debug == 1)printf("SRA E\n");
			SRA(&CPU.E);
			break;
		case 0x2c:
			if (debug == 1)printf("SRA H\n");
			SRA(&CPU.H);
			break;
		case 0x2d:
			if (debug == 1)printf("SRA L\n");
			SRA(&CPU.L);
			break;
		case 0x2e:
			if (debug == 1)printf("SRA (HL)\n");
			HLm = readMem(HL);
			SRA(&HLm);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0x2f:
			if (debug == 1)printf("SRA A\n");
			SRA(&CPU.A);
			break;
		case 0x30:
			if (debug == 1)printf("SWAP B \n");
			SWAP(&CPU.B);
			break;
		case 0x31:
			if (debug == 1)printf("SWAP C \n");
			SWAP(&CPU.C);
			break;
		case 0x32:
			if (debug == 1)printf("SWAP D \n");
			SWAP(&CPU.D);
			break;
		case 0x33:
			if (debug == 1)printf("SWAP E \n");
			SWAP(&CPU.E);
			break;
		case 0x34:
			if (debug == 1)printf("SWAP H \n");
			SWAP(&CPU.H);
			break;
		case 0x35:
			if (debug == 1)printf("SWAP L \n");
			SWAP(&CPU.L);
			break;
		case 0x36:
			if (debug == 1)printf("SWAP (HL)\n");
			HLm = readMem(HL);
			SWAP(&HLm);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0x37:	
			if(debug == 1)printf("SWAP A \n");
			SWAP(&CPU.A);
			break;
		case 0x38:
			if (debug == 1)printf("SRL B \n");
			SRL(&CPU.B);
			break;
		case 0x39:
			if (debug == 1)printf("SRL C \n");
			SRL(&CPU.C);
			break;
		case 0x3a:
			if (debug == 1)printf("SRL D \n");
			SRL(&CPU.D);
			break;
		case 0x3b:
			if (debug == 1)printf("SRL E \n");
			SRL(&CPU.E);
			break;
		case 0x3c:
			if (debug == 1)printf("SRL H \n");
			SRL(&CPU.H);
			break;
		case 0x3d:
			if (debug == 1)printf("SRL L \n");
			SRL(&CPU.L);
			break;
		case 0x3e:
			if (debug == 1)printf("SRL (HL)\n");
			HLm = readMem(HL);
			SRL(&HLm);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0x3f:
			if (debug == 1)printf("SRL A \n");
			SRL(&CPU.A);
			break;
		case 0x40:
			if (debug == 1)printf("BIT 0,B \n");
			BIT(CPU.B, 0);
			break;
		case 0x41:
			if (debug == 1)printf("BIT 0,C \n");
			BIT(CPU.C, 0);
			break;
		case 0x42:  
			if (debug == 1)printf("BIT 0,D \n");
			BIT(CPU.D, 0);
			break;
		case 0x43:
			if (debug == 1)printf("BIT 0,E \n");
			BIT(CPU.E, 0);
			break;
		case 0x44:
			if (debug == 1)printf("BIT 0,H \n");
			BIT(CPU.H, 0);
			break;
		case 0x45:
			if (debug == 1)printf("BIT 0,L \n");
			BIT(CPU.L, 0);
			break;
		case 0x46:
			if (debug == 1)printf("BIT 0,(HL) \n");
			HLm = readMem(HL);
			BIT(HLm,0);
			writeMem(HL, HLm);
			CPU.CPUTicks += 4;
			break;
		case 0x47:  
			if (debug == 1)printf("BIT 0,A \n");
			BIT(CPU.A, 0);
			break;
		case 0x48:
			if (debug == 1)printf("BIT 1,B \n");
			BIT(CPU.B, 1);
			break;
		case 0x49:
			if (debug == 1)printf("BIT 1,C \n");
			BIT(CPU.C, 1);
			break;
		case 0x4a:
			if (debug == 1)printf("BIT 1,D \n");
			BIT(CPU.D, 1);
			break;
		case 0x4b:
			if (debug == 1)printf("BIT 1,E \n");
			BIT(CPU.E, 1);
			break;
		case 0x4c:
			if (debug == 1)printf("BIT 1,H \n");
			BIT(CPU.H, 1);
			break;
		case 0x4d:
			if (debug == 1)printf("BIT 1,L \n");
			BIT(CPU.L, 1);
			break;
		case 0x4e:
			if (debug == 1)printf("BIT 1,(HL) \n");
			HLm = readMem(HL);
			BIT(HLm, 1);
			writeMem(HL, HLm);
			CPU.CPUTicks += 4;
			break;
		case 0x4f:
			if (debug == 1)printf("BIT 1,A \n");
			BIT(CPU.A, 1);
			break;
		case 0x50:
			if (debug == 1)printf("BIT 2,B \n");
			BIT(CPU.B, 2);
			break;
		case 0x51:
			if (debug == 1)printf("BIT 2,C \n");
			BIT(CPU.C, 2);
			break;
		case 0x52:
			if (debug == 1)printf("BIT 2,D \n");
			BIT(CPU.D, 2);
			break;
		case 0x53:
			if (debug == 1)printf("BIT 2,E \n");
			BIT(CPU.E, 2);
			break;
		case 0x54:
			if (debug == 1)printf("BIT 2,H \n");
			BIT(CPU.H, 2);
			break;
		case 0x55:
			if (debug == 1)printf("BIT 2,L \n");
			BIT(CPU.L, 2);
			break;
		case 0x56:
			if (debug == 1)printf("BIT 2,(HL)\n");
			HLm = readMem(HL);
			BIT(HLm, 2);
			writeMem(HL, HLm);
			CPU.CPUTicks += 4;
			break;
		case 0x57:
			if (debug == 1)printf("BIT 2,A \n");
			BIT(CPU.A, 2);
			break;
		case 0x58:
			if (debug == 1)printf("BIT 3,B \n");
			BIT(CPU.B, 3);
			break;
		case 0x59:
			if (debug == 1)printf("BIT 3,C \n");
			BIT(CPU.C, 3);
			break;
		case 0x5a:
			if (debug == 1)printf("BIT 3,D \n");
			BIT(CPU.D, 3);
			break;
		case 0x5b:
			if (debug == 1)printf("BIT 3,E \n");
			BIT(CPU.E, 3);
			break;
		case 0x5c:
			if (debug == 1)printf("BIT 3,H \n");
			BIT(CPU.H, 3);
			break;
		case 0x5d:
			if (debug == 1)printf("BIT 3,L \n");
			BIT(CPU.L, 3);
			break;
		case 0x5e:
			if (debug == 1)printf("BIT 3,(HL)\n");
			HLm = readMem(HL);
			BIT(HLm, 3);
			writeMem(HL, HLm);
			CPU.CPUTicks += 4;
			break;
		case 0x5f:
			if (debug == 1)printf("BIT 3,A \n");
			BIT(CPU.A, 3);
			break;
		case 0x60:
			if (debug == 1)printf("BIT 4,B \n");
			BIT(CPU.B, 4);
			break;
		case 0x61:
			if (debug == 1)printf("BIT 4,C \n");
			BIT(CPU.C, 4);
			break;
		case 0x62:
			if (debug == 1)printf("BIT 4,D \n");
			BIT(CPU.D, 4);
			break;
		case 0x63:
			if (debug == 1)printf("BIT 4,E \n");
			BIT(CPU.E, 4);
			break;
		case 0x64:
			if (debug == 1)printf("BIT 4,H \n");
			BIT(CPU.H, 4);
			break;
		case 0x65:
			if (debug == 1)printf("BIT 4,L \n");
			BIT(CPU.L, 4);
			break;
		case 0x66:
			if (debug == 1)printf("BIT 4,(HL)\n");
			HLm = readMem(HL);
			BIT(HLm, 4);
			writeMem(HL, HLm);
			CPU.CPUTicks += 4;
			break;
		case 0x67:
			if (debug == 1)printf("BIT 4,A \n");
			BIT(CPU.A, 4);
			break;
		case 0x68:
			if (debug == 1)printf("BIT 5,B \n");
			BIT(CPU.B, 5);
			break;
		case 0x69:
			if (debug == 1)printf("BIT 5,C \n");
			BIT(CPU.C, 5);
			break;
		case 0x6a:
			if (debug == 1)printf("BIT 5,D \n");
			BIT(CPU.D, 5);
			break;
		case 0x6b:
			if (debug == 1)printf("BIT 5,E \n");
			BIT(CPU.E, 5);
			break;
		case 0x6c:
			if (debug == 1)printf("BIT 5,H \n");
			BIT(CPU.H, 5);
			break;
		case 0x6d:
			if (debug == 1)printf("BIT 5,L \n");
			BIT(CPU.L, 5);
			break;
		case 0x6e:
			if (debug == 1)printf("BIT 5,(HL)\n");
			HLm = readMem(HL);
			BIT(HLm, 5);
			writeMem(HL, HLm);
			CPU.CPUTicks += 4;
			break;
		case 0x6f:
			if (debug == 1)printf("BIT 5,A \n");
			BIT(CPU.A, 5);
			break;
		case 0x70:
			if (debug == 1)printf("BIT 6,B \n");
			BIT(CPU.B, 6);
			break;
		case 0x71:
			if (debug == 1)printf("BIT 6,C \n");
			BIT(CPU.C, 6);
			break;
		case 0x72:
			if (debug == 1)printf("BIT 6,D \n");
			BIT(CPU.D, 6);
			break;
		case 0x73:
			if (debug == 1)printf("BIT 6,E \n");
			BIT(CPU.E, 6);
			break;
		case 0x74:
			if (debug == 1)printf("BIT 6,H \n");
			BIT(CPU.H, 6);
			break;
		case 0x75:
			if (debug == 1)printf("BIT 6,L \n");
			BIT(CPU.L, 6);
			break;
		case 0x76:
			if (debug == 1)printf("BIT 6,(HL)\n");
			HLm = readMem(HL);
			BIT(HLm, 6);
			writeMem(HL, HLm);
			CPU.CPUTicks += 4;
			break;
		case 0x77:
			if (debug == 1)printf("BIT 6,A \n");
			BIT(CPU.A, 6);
			break;
		case 0x78:
			if (debug == 1)printf("BIT 7,B\n");
			BIT(CPU.B, 7);
			break;
		case 0x79:
			if (debug == 1)printf("BIT 7,C\n");
			BIT(CPU.C, 7);
			break;
		case 0x7a:
			if (debug == 1)printf("BIT 7,D\n");
			BIT(CPU.D, 7);
			break;
		case 0x7b:
			if (debug == 1)printf("BIT 7,E\n");
			BIT(CPU.E, 7);
			break;
		case 0x7c:  
			if (debug == 1)printf("BIT 7,H \n");
			BIT(CPU.H, 7);
			break;
		case 0x7d:
			if (debug == 1)printf("BIT 7,L\n");
			BIT(CPU.L, 7);
			break;
		case 0x7e:
			if (debug == 1)printf("BIT 7,(HL)\n");
			HLm = readMem(HL);
			BIT(HLm, 7);
			writeMem(HL, HLm);
			CPU.CPUTicks += 4;
			break;
		case 0x7f:
			if (debug == 1)printf("BIT 7,A \n");
			BIT(CPU.A, 7);
			break;
		case 0x80:
			if (debug == 1)printf("RES 0,B \n");
			RES(&CPU.B, 0);
			break;
		case 0x81:
			if (debug == 1)printf("RES 0,C \n");
			RES(&CPU.C, 0);
			break;
		case 0x82:
			if (debug == 1)printf("RES 0,D \n");
			RES(&CPU.D, 0);
			break;
		case 0x83:
			if (debug == 1)printf("RES 0,E \n");
			RES(&CPU.E, 0);
			break;
		case 0x84:
			if (debug == 1)printf("RES 0,H \n");
			RES(&CPU.H, 0);
			break;
		case 0x85:
			if (debug == 1)printf("RES 0,L \n");
			RES(&CPU.L, 0);
			break;
		case 0x86:
			if (debug == 1)printf("RES 0,(HL) \n");
			HLm = readMem(HL);
			RES(&HLm,0);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0x87:	
			if (debug == 1)printf("RES 0,A \n");
			RES(&CPU.A, 0);
			break;
		case 0x88:
			if (debug == 1)printf("RES 1,B \n");
			RES(&CPU.B, 1);
			break;
		case 0x89:
			if (debug == 1)printf("RES 1,C \n");
			RES(&CPU.C, 1);
			break;
		case 0x8a:
			if (debug == 1)printf("RES 1,D \n");
			RES(&CPU.D, 1);
			break;
		case 0x8b:
			if (debug == 1)printf("RES 1,E \n");
			RES(&CPU.E, 1);
			break;
		case 0x8c:
			if (debug == 1)printf("RES 1,H \n");
			RES(&CPU.H, 1);
			break;
		case 0x8d:
			if (debug == 1)printf("RES 1,L \n");
			RES(&CPU.L, 1);
			break;
		case 0x8e:
			if (debug == 1)printf("RES 1,(HL) \n");
			HLm = readMem(HL);
			RES(&HLm, 1);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0x8f:
			if (debug == 1)printf("RES 1,A \n");
			RES(&CPU.A, 1);
			break;
		case 0x90:
			if (debug == 1)printf("RES 2,B \n");
			RES(&CPU.B, 2);
			break;
		case 0x91:
			if (debug == 1)printf("RES 2,C \n");
			RES(&CPU.C, 2);
			break;
		case 0x92:
			if (debug == 1)printf("RES 2,D \n");
			RES(&CPU.D, 2);
			break;
		case 0x93:
			if (debug == 1)printf("RES 2,E \n");
			RES(&CPU.E, 2);
			break;
		case 0x94:
			if (debug == 1)printf("RES 2,H \n");
			RES(&CPU.H, 2);
			break;
		case 0x95:
			if (debug == 1)printf("RES 2,L \n");
			RES(&CPU.L, 2);
			break;
		case 0x96:
			if (debug == 1)printf("RES 2,(HL) \n");
			HLm = readMem(HL);
			RES(&HLm, 2);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0x97:
			if(debug == 1)printf("RES 2,A \n");
			RES(&CPU.A, 2);
			break;
		case 0x98:
			if (debug == 1)printf("RES 3,B \n");
			RES(&CPU.B, 3);
			break;
		case 0x99:
			if (debug == 1)printf("RES 3,C \n");
			RES(&CPU.C, 3);
			break;
		case 0x9A:
			if (debug == 1)printf("RES 3,D \n");
			RES(&CPU.D, 3);
			break;
		case 0x9B:
			if (debug == 1)printf("RES 3,E \n");
			RES(&CPU.E, 3);
			break;
		case 0x9C:
			if (debug == 1)printf("RES 3,H \n");
			RES(&CPU.H, 3);
			break;
		case 0x9D:
			if (debug == 1)printf("RES 3,L \n");
			RES(&CPU.L, 3);
			break;
		case 0x9E:
			if (debug == 1)printf("RES 3,(HL) \n");
			HLm = readMem(HL);
			RES(&HLm, 3);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0x9F:
			if (debug == 1)printf("RES 3,A \n");
			RES(&CPU.A, 3);
			break;
		case 0xa0:
			if (debug == 1)printf("RES 4,B \n");
			RES(&CPU.B, 4);
			break;
		case 0xa1:
			if (debug == 1)printf("RES 4,C \n");
			RES(&CPU.C, 4);
			break;
		case 0xa2:
			if (debug == 1)printf("RES 4,D \n");
			RES(&CPU.D, 4);
			break;
		case 0xa3:
			if (debug == 1)printf("RES 4,E \n");
			RES(&CPU.E, 4);
			break;
		case 0xa4:
			if (debug == 1)printf("RES 4,H \n");
			RES(&CPU.H, 4);
			break;
		case 0xa5:
			if (debug == 1)printf("RES 4,L \n");
			RES(&CPU.L, 4);
			break;
		case 0xa6:
			if (debug == 1)printf("RES 4,(HL) \n");
			HLm = readMem(HL);
			RES(&HLm, 4);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0xa7:
			if (debug == 1)printf("RES 4,A \n");
			RES(&CPU.A, 4);
			break;
		case 0xa8:
			if (debug == 1)printf("RES 5,B \n");
			RES(&CPU.B, 5);
			break;
		case 0xa9:
			if (debug == 1)printf("RES 5,C \n");
			RES(&CPU.C, 5);
			break;
		case 0xaa:
			if (debug == 1)printf("RES 5,D \n");
			RES(&CPU.D, 5);
			break;
		case 0xab:
			if (debug == 1)printf("RES 5,E \n");
			RES(&CPU.E, 5);
			break;
		case 0xac:
			if (debug == 1)printf("RES 5,H \n");
			RES(&CPU.H, 5);
			break;
		case 0xad:
			if (debug == 1)printf("RES 5,L \n");
			RES(&CPU.L, 5);
			break;
		case 0xae:
			if (debug == 1)printf("RES 5,(HL) \n");
			HLm = readMem(HL);
			RES(&HLm, 5);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0xaf:
			if (debug == 1)printf("RES 5,A \n");
			RES(&CPU.A, 5);
			break;
		case 0xb0:
			if (debug == 1)printf("RES 6,B \n");
			RES(&CPU.B, 6);
			break;
		case 0xb1:
			if (debug == 1)printf("RES 6,C \n");
			RES(&CPU.C, 6);
			break;
		case 0xb2:
			if (debug == 1)printf("RES 6,D \n");
			RES(&CPU.D, 6);
			break;
		case 0xb3:
			if (debug == 1)printf("RES 6,E \n");
			RES(&CPU.E, 6);
			break;
		case 0xb4:
			if (debug == 1)printf("RES 6,H \n");
			RES(&CPU.H, 6);
			break;
		case 0xb5:
			if (debug == 1)printf("RES 6,L \n");
			RES(&CPU.L, 6);
			break;
		case 0xb6:
			if (debug == 1)printf("RES 6,(HL) \n");
			HLm = readMem(HL);
			RES(&HLm, 6);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0xb7:
			if (debug == 1)printf("RES 6,A \n");
			RES(&CPU.A, 6);
			break;
		case 0xb8:
			if (debug == 1)printf("RES 7,B \n");
			RES(&CPU.B, 7);
			break;
		case 0xb9:
			if (debug == 1)printf("RES 7,C \n");
			RES(&CPU.C, 7);
			break;
		case 0xba:
			if (debug == 1)printf("RES 7,D \n");
			RES(&CPU.D, 7);
			break;
		case 0xbb:
			if (debug == 1)printf("RES 7,E \n");
			RES(&CPU.E, 7);
			break;
		case 0xbc:
			if (debug == 1)printf("RES 7,H \n");
			RES(&CPU.H, 7);
			break;
		case 0xbd:
			if (debug == 1)printf("RES 7,L \n");
			RES(&CPU.L, 7);
			break;
		case 0xbe:
			if (debug == 1)printf("RES 7,(HL) \n");
			HLm = readMem(HL);
			RES(&HLm, 7);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0xbf:
			if (debug == 1)printf("RES 7,A \n");
			RES(&CPU.A, 7);
			break;
		case 0xc0:
			if (debug == 1)printf("SET 0,B \n");
			SET(&CPU.B, 0);
			break;
		case 0xc1:
			if (debug == 1)printf("SET 0,C \n");
			SET(&CPU.C, 0);
			break;
		case 0xc2:
			if (debug == 1)printf("SET 0,D \n");
			SET(&CPU.D, 0);
			break;
		case 0xc3:
			if (debug == 1)printf("SET 0,E \n");
			SET(&CPU.E, 0);
			break;
		case 0xc4:
			if (debug == 1)printf("SET 0,H \n");
			SET(&CPU.H, 0);
			break;
		case 0xc5:
			if (debug == 1)printf("SET 0,L \n");
			SET(&CPU.L, 0);
			break;
		case 0xc6:
			if (debug == 1)printf("SET 0,(HL) \n");
			HLm = readMem(HL);
			SET(&HLm, 0);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0xc7:
			if (debug == 1)printf("SET 0,A \n");
			SET(&CPU.A, 0);
			break;
		case 0xc8:
			if (debug == 1)printf("SET 1,B \n");
			SET(&CPU.B, 1);
			break;
		case 0xc9:
			if (debug == 1)printf("SET 1,C \n");
			SET(&CPU.C, 1);
			break;
		case 0xca:
			if (debug == 1)printf("SET 1,D \n");
			SET(&CPU.D, 1);
			break;
		case 0xcb:
			if (debug == 1)printf("SET 1,E \n");
			SET(&CPU.E, 1);
			break;
		case 0xcc:
			if (debug == 1)printf("SET 1,H \n");
			SET(&CPU.H, 1);
			break;
		case 0xcd:
			if (debug == 1)printf("SET 1,L \n");
			SET(&CPU.L, 1);
			break;
		case 0xce:
			if (debug == 1)printf("SET 1,(HL) \n");
			HLm = readMem(HL);
			SET(&HLm, 1);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0xcf:	
			if (debug == 1)printf("SET 1,A \n");
			SET(&CPU.A, 1);
			break;
		case 0xd0:
			if (debug == 1)printf("SET 2,B \n");
			SET(&CPU.B, 2);
			break;
		case 0xd1:
			if (debug == 1)printf("SET 2,C \n");
			SET(&CPU.C, 2);
			break;
		case 0xd2:
			if (debug == 1)printf("SET 2,D \n");
			SET(&CPU.D, 2);
			break;
		case 0xd3:
			if (debug == 1)printf("SET 2,E \n");
			SET(&CPU.E, 2);
			break;
		case 0xd4:
			if (debug == 1)printf("SET 2,H \n");
			SET(&CPU.H, 2);
			break;
		case 0xd5:
			if (debug == 1)printf("SET 2,L \n");
			SET(&CPU.L, 2);
			break;
		case 0xd6:
			if (debug == 1)printf("SET 2,(HL) \n");
			HLm = readMem(HL);
			SET(&HLm, 2);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0xd7:
			if (debug == 1)printf("SET 2,A \n");
			SET(&CPU.A, 2);
			break;
		case 0xd8:
			if (debug == 1)printf("SET 3,B \n");
			SET(&CPU.B, 3);
			break;
		case 0xd9:
			if (debug == 1)printf("SET 3,C \n");
			SET(&CPU.C, 3);
			break;
		case 0xda:
			if (debug == 1)printf("SET 3,D \n");
			SET(&CPU.D, 3);
			break;
		case 0xdb:
			if (debug == 1)printf("SET 3,E \n");
			SET(&CPU.E, 3);
			break;
		case 0xdc:
			if (debug == 1)printf("SET 3,H \n");
			SET(&CPU.H, 3);
			break;
		case 0xdd:
			if (debug == 1)printf("SET 3,L \n");
			SET(&CPU.L, 3);
			break;
		case 0xde:
			if (debug == 1)printf("SET 3,(HL) \n");
			HLm = readMem(HL);
			SET(&HLm, 3);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0xdf:
			if (debug == 1)printf("SET 3,A \n");
			SET(&CPU.A, 3);
			break;
		case 0xe0:
			if (debug == 1)printf("SET 4,B \n");
			SET(&CPU.B, 4);
			break;
		case 0xe1:
			if (debug == 1)printf("SET 4,C \n");
			SET(&CPU.C, 4);
			break;
		case 0xe2:
			if (debug == 1)printf("SET 4,D \n");
			SET(&CPU.D, 4);
			break;
		case 0xe3:
			if (debug == 1)printf("SET 4,E \n");
			SET(&CPU.E, 4);
			break;
		case 0xe4:
			if (debug == 1)printf("SET 4,H \n");
			SET(&CPU.H, 4);
			break;
		case 0xe5:
			if (debug == 1)printf("SET 4,L \n");
			SET(&CPU.L, 4);
			break;
		case 0xe6:
			if (debug == 1)printf("SET 4,(HL) \n");
			HLm = readMem(HL);
			SET(&HLm, 4);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0xe7:
			if (debug == 1)printf("SET 4,A \n");
			SET(&CPU.A, 4);
			break;
		case 0xe8:
			if (debug == 1)printf("SET 5,B \n");
			SET(&CPU.B, 5);
			break;
		case 0xe9:
			if (debug == 1)printf("SET 5,C \n");
			SET(&CPU.C, 5);
			break;
		case 0xea:
			if (debug == 1)printf("SET 5,D \n");
			SET(&CPU.D, 5);
			break;
		case 0xeb:
			if (debug == 1)printf("SET 5,E \n");
			SET(&CPU.E, 5);
			break;
		case 0xec:
			if (debug == 1)printf("SET 5,H \n");
			SET(&CPU.H, 5);
			break;
		case 0xed:
			if (debug == 1)printf("SET 5,L \n");
			SET(&CPU.L, 5);
			break;
		case 0xee:
			if (debug == 1)printf("SET 5,(HL) \n");
			HLm = readMem(HL);
			SET(&HLm, 5);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0xef:
			if (debug == 1)printf("SET 5,A \n");
			SET(&CPU.A, 5);
			break;
		case 0xf0:
			if (debug == 1)printf("SET 6,B \n");
			SET(&CPU.B, 6);
			break;
		case 0xf1:
			if (debug == 1)printf("SET 6,C \n");
			SET(&CPU.C, 6);
			break;
		case 0xf2:
			if (debug == 1)printf("SET 6,D \n");
			SET(&CPU.D, 6);
			break;
		case 0xf3:
			if (debug == 1)printf("SET 6,E \n");
			SET(&CPU.E, 6);
			break;
		case 0xf4:
			if (debug == 1)printf("SET 6,H \n");
			SET(&CPU.H, 6);
			break;
		case 0xf5:
			if (debug == 1)printf("SET 6,L \n");
			SET(&CPU.L, 6);
			break;
		case 0xf6:
			if (debug == 1)printf("SET 6,(HL) \n");
			HLm = readMem(HL);
			SET(&HLm, 6);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0xf7:
			if (debug == 1)printf("SET 6,A \n");
			SET(&CPU.A, 6);
			break;
		case 0xf8:
			if (debug == 1)printf("SET 7,B \n");
			SET(&CPU.B, 7);
			break;
		case 0xf9:
			if (debug == 1)printf("SET 7,C \n");
			SET(&CPU.C, 7);
			break;
		case 0xfa:
			if (debug == 1)printf("SET 7,D \n");
			SET(&CPU.D, 7);
			break;
		case 0xfb:
			if (debug == 1)printf("SET 7,E \n");
			SET(&CPU.E, 7);
			break;
		case 0xfc:
			if (debug == 1)printf("SET 7,H \n");
			SET(&CPU.H, 7);
			break;
		case 0xfd:
			if (debug == 1)printf("SET 7,L \n");
			SET(&CPU.L, 7);
			break;
		case 0xfe:
			if (debug == 1)printf("SET 7,(HL) \n");
			HLm = readMem(HL);
			SET(&HLm, 7);
			writeMem(HL, HLm);
			CPU.CPUTicks += 8;
			break;
		case 0xff:
			if (debug == 1)printf("SET 7,A \n");
			SET(&CPU.A, 7);
			break;
		default:
			printf("Unimplemented CB %X @ PC:%X\n", op, CPU.PC - 2);
			getchar();
			break;
	}
}

ushort getPC() {
	return CPU.PC;
}

ushort getSP() {
	return CPU.SP;
}

void setPC(ushort A) {
	CPU.PC = A;
}

uchar getIME() {
	return CPU.IME;
}

void setIME(uchar A) {
	CPU.IME = A;
}

ulong getCPUTicks(){
	return CPU.CPUTicks;
}

void halted(uchar a) {
	CPU.halt = a & 0x1;
}

uchar DIHalt() {
	return(CPU.halt && (CPU.IME == 0));
}

ushort getReg(uchar A) {
	switch (A) {
	default:return AF;
	case 1:return BC;
	case 2:return DE;
	case 3:return HL;
	}
}

ushort adr16() {
	ushort adrl = OP;
	ushort adrh = OP;
	return adrh << 8 | adrl;
}

void pushPC() {
	//decrement SP then place data
	writeMem(--CPU.SP, (CPU.PC >> 8) & 0xFF);//write upper
	writeMem(--CPU.SP, CPU.PC & 0xFF);//write lower
	CPU.CPUTicks += 4;
	CPU.CPUTicks += 4;
	CPU.CPUTicks += 4;
}

void popPC() {
	//write then increment SP
	CPU.PC = readMem(CPU.SP++);
	CPU.PC |= readMem(CPU.SP++) << 8;
	CPU.CPUTicks += 4;
	CPU.CPUTicks += 4;
	CPU.CPUTicks += 4;
}

void pushReg(ushort A) {
	//decrement SP then place data
	writeMem(--CPU.SP, (A >> 8) & 0xFF);//write upper
	writeMem(--CPU.SP, A & 0xFF);//write lower
	CPU.CPUTicks += 4;
	CPU.CPUTicks += 4;
	CPU.CPUTicks += 4;
	CPU.CPUTicks += 4;
}

void AND(uchar B) {
	Flag = 0x20;
	CPU.A &= B;
	if (CPU.A == 0)       //Zero Flag
		Flag |= 0x80;
	CPU.CPUTicks += 4;
}

void OR(uchar B) {
	CPU.A |= B;
	if (CPU.A == 0)       //Zero Flag
		Flag = 0x80;
	else
		Flag = 0x0;
	CPU.CPUTicks += 4;
}

void XOR(uchar B) {
	CPU.A ^= B;
	if (CPU.A == 0)       //Zero Flag
		Flag = 0x80;
	else
		Flag = 0x0;
	CPU.CPUTicks += 4;
}

void CPL() {
	CPU.A = ~CPU.A;
	Flag |= 0x60;
	CPU.CPUTicks += 4;
}

void SCF() {
	Flag &= 0x80;//keep Z
	Flag |= 0x10;//set c
	CPU.CPUTicks += 4;
}

void CCF() {
	Flag = (FlagZ << 7) | ((FlagC ^ 0x1) << 4) & 0xF0;//keep z, comp c, clear rest
	CPU.CPUTicks += 4;
}

void ADD(uchar B) {
	Flag = 0;
	ushort C = CPU.A + B;
	if (C > 0xFF)//carry flag
		Flag |= 0x10;

	if ((CPU.A & 0x0F) + (B & 0x0F) > 0x0F)//half carry
		Flag |= 0x20;

	CPU.A += B;
	if (CPU.A == 0)       //Zero Flag
		Flag |= 0x80;
	CPU.CPUTicks += 4;
}

void SUB(uchar B) {
	Flag = 0x40;//Set Sub flag

	//Half Carry Subtraction
	if ((CPU.A & 0xF) < (B & 0xF))
		Flag |= 0x20;

	//borrow flag
	if (CPU.A < B)
		Flag |= 0x10;

	CPU.A -= B;
	if (CPU.A == 0)       //Zero Flag
		Flag |= 0x80;
	CPU.CPUTicks += 4;
}

void ADC(uchar B) {
	uchar D = FlagC;
	Flag = 0;
	ushort C = CPU.A + B + D;
	if (C > 0xFF)//carry flag
		Flag |= 0x10;

	if (((CPU.A & 0x0F) + (B & 0x0F) + D) > 0x0F)//half carry
		Flag |= 0x20;

	CPU.A += B + D;
	if (CPU.A == 0)       //Zero Flag
		Flag |= 0x80;
	CPU.CPUTicks += 4;
}

void SBC(uchar B) {
	uchar C = FlagC;
	Flag = 0x40;//Set Sub flag

	//Half Carry Subtraction
	if ((CPU.A & 0xF) < ((B & 0xF) + C))
		Flag |= 0x20;

	//borrow flag
	if (CPU.A < (B + C))
		Flag |= 0x10;

	CPU.A -= B + C;
	if (CPU.A == 0)       //Zero Flag
		Flag |= 0x80;
	CPU.CPUTicks += 4;
}

void ADDHL(ushort B) {
	
	ulong C = HL + B;
	if (C > 0xFFFF)//carry flag
		Flag |= 0x10;
	else
		Flag &= ~0x10;

	if (((HL & 0x0FFF) + (B & 0x0FFF)) > 0x0FFF)//half carry
		Flag |= 0x20;
	else
		Flag &= ~0x20;

	Flag &= ~0x40; //clear sub flag

	CPU.H = (C >> 8) & 0xFF;
	CPU.L = C & 0xFF;
	CPU.CPUTicks += 4;
	CPU.CPUTicks += 4;
}

void INC(uchar * A) {
	Flag &= ~0x40; //clear sub flag

	if ((*A & 0x0F) + 1 > 0x0F)//half carry
		Flag |= 0x20;
	else
		Flag &= ~0x20;

	*A += 1;

	if (*A == 0)       //Zero Flag
		Flag |= 0x80;
	else
		Flag &= ~0x80;
	CPU.CPUTicks += 4;
}

void DEC(uchar * A) {
	Flag |= 0x40;//Set Sub flag

	if ((*A & 0x0F) == 0)//half carry
		Flag |= 0x20;
	else
		Flag &= ~0x20;
	/*// Dec doesn't use borrow flag
	if (*A == 0)//borrow
		Flag |= 0x10;
	else
		Flag &= ~0x10;*/

	*A -= 1;
	if (*A == 0)       //Zero Flag
		Flag |= 0x80;
	else
		Flag &= ~0x80;
	CPU.CPUTicks += 4;
}

void INC2(uchar * A, uchar * B) {
	//no flags updated
	ushort C;
	C = (*A << 8) | *B;
	C += 1;
	*A = (C >> 8) & 0xFF;
	*B = C & 0xFF;
	CPU.CPUTicks += 4;
	CPU.CPUTicks += 4;
}

void DEC2(uchar * A, uchar * B) {
	//no flags updated
	ushort C;
	C = (*A << 8) | *B;
	C -= 1;
	*A = (C >> 8) & 0xFF;
	*B = C & 0xFF;
	CPU.CPUTicks += 4;
	CPU.CPUTicks += 4;
}

void INCHL() {
	Flag &= ~0x40; //clear sub flag
	uchar A = readMem(HL);

	if ((A & 0x0F) + 1 > 0x0F)//half carry
		Flag |= 0x20;
	else
		Flag &= ~0x20;

	A += 1;

	if (A == 0)       //Zero Flag
		Flag |= 0x80;
	else
		Flag &= ~0x80;
	writeMem(HL, A);
	CPU.CPUTicks += 4;
	CPU.CPUTicks += 4;
	CPU.CPUTicks += 4;
}

void DECHL() {
	Flag |= 0x40;//Set Sub flag
	uchar A = readMem(HL);

	if ((A & 0x0F) == 0)//half carry
		Flag |= 0x20;
	else
		Flag &= ~0x20;
	/*
	if (A == 0)//borrow
		Flag |= 0x10;
	else
		Flag &= ~0x10;*/

	A -= 1;
	if (A == 0)       //Zero Flag
		Flag |= 0x80;
	else
		Flag &= ~0x80;
	writeMem(HL, A);
	CPU.CPUTicks += 4;
	CPU.CPUTicks += 4;
	CPU.CPUTicks += 4;
}

void CP(uchar B) {
	Flag = 0x40;//Set Sub flag

	//Half Carry Subtraction
	if ((CPU.A & 0xF) < (B & 0xF))
		Flag |= 0x20;

	//borrow flag
	if (CPU.A < B)
		Flag |= 0x10;

	if (CPU.A == B)       //Zero Flag
		Flag |= 0x80;
	CPU.CPUTicks += 4;
}

void DAA() {
	/*if (FlagN == 0)
	{
		if ((FlagC == 1) || (CPU.A > 0x99))
		{
			CPU.A += 0x60;
			Flag |= 0x10;
		}
		if ((FlagH == 1) || ((CPU.A & 0x0F) > 0x09))
		{
			CPU.A += 0x06;
			Flag &= ~0x20;
		}
	}
	else if (FlagC && FlagH)
	{
		CPU.A += 0x9A;
		Flag &= ~0x20;
	}
	else if (FlagC)
	{
		CPU.A += 0xA0;
	}
	else if (FlagH)
	{
		CPU.A += 0xFA;
		Flag &= ~0x20;
	}

	if (CPU.A == 0)
		Flag |= 0x80;
	else
		Flag &= ~0x80;
	/**/if (FlagN)
	{
		if (FlagC) 
			CPU.A -= 0x60;
		if (FlagH) 
			CPU.A -= 0x06;
	}
	else
	{
		if (FlagC || (CPU.A & 0xFF) > 0x99)
		{		
			CPU.A += 0x60;
			Flag |= 0x10;
		}
		if (FlagH || (CPU.A & 0x0F) > 0x09)
		{ 
			CPU.A += 0x06; 
		}
	}

	if (CPU.A == 0)
		Flag |= 0x80;
	else
		Flag &= ~0x80;
	
	Flag &= ~0x20;

	CPU.CPUTicks += 4;
}

void ADDSP(uchar B)
{
	Flag = 0;
	ushort sum = CPU.SP + ((B > 0x7F) ? (B | 0xFF00) : B);//sign extension
	if (((CPU.SP & 0xFF) + B) > 0xFF)
		Flag |= 0x10;
	if (((CPU.SP & 0x0F) + (B & 0x0F)) > 0x0F)
		Flag |= 0x20;
	CPU.SP = sum;
	CPU.CPUTicks += 4;
	CPU.CPUTicks += 4;
	CPU.CPUTicks += 4;
	CPU.CPUTicks += 4;
}

void LDHLSP(uchar B)
{
	Flag = 0;
	ushort sum = CPU.SP + ((B > 0x7F) ? (B | 0xFF00) : B);//sign extension
	if (((CPU.SP & 0xFF) + B) > 0xFF)
		Flag |= 0x10;
	if (((CPU.SP & 0x0F) + (B & 0x0F)) > 0x0F)
		Flag |= 0x20;

	CPU.H = (sum >> 8) & 0xFF;
	CPU.L = sum & 0xFF;
	CPU.CPUTicks += 4;
	CPU.CPUTicks += 4;
	CPU.CPUTicks += 4;
}

///------------------------------------------------
//EXtended OpCodes

void RL(uchar *A) {
	uchar temp = FlagC;
	Flag = (*A >> 7) << 4;
	*A = *A << 1;
	*A |= temp;
	if (*A == 0)
		Flag |= 0x80;
	CPU.CPUTicks += 4;
}

void RLC(uchar *A) {
	uchar temp = (*A >> 7);
	Flag = temp << 4;
	*A = *A << 1;
	*A |= temp;
	if (*A == 0)
		Flag |= 0x80;
	CPU.CPUTicks += 4;
}

void RLA() {
	uchar temp = FlagC;
	Flag = ((CPU.A >> 7) << 4) & 0x10;
	CPU.A = CPU.A << 1;
	CPU.A |= temp;
	/*if (CPU.A == 0)
		Flag |= 0x80;*/
	CPU.CPUTicks += 4;
}

void RLCA() {
	uchar temp = (CPU.A >> 7);
	Flag = temp << 4;
	CPU.A = CPU.A << 1;
	CPU.A |= temp;
	/*if (CPU.A == 0)
		Flag |= 0x80;*/
	CPU.CPUTicks += 4;
}

void RR(uchar *A) {
	uchar temp = FlagC;
	Flag = (*A & 0x1) << 4;
	*A = *A >> 1;
	*A |= (temp << 7);
	if (*A == 0)
		Flag |= 0x80;
	CPU.CPUTicks += 4;
}

void RRC(uchar *A) {
	uchar temp = (*A & 0x1);
	Flag = temp << 4;
	*A = *A >> 1;
	*A |= (temp << 7);
	if (*A == 0)
		Flag |= 0x80;
	CPU.CPUTicks += 4;
}

void RRA() {
	uchar temp = FlagC;
	Flag = (CPU.A & 0x1) << 4;
	CPU.A = CPU.A >> 1;
	CPU.A |= (temp << 7);
	/*if (CPU.A == 0)
		Flag |= 0x80;*/
	CPU.CPUTicks += 4;
}

void RRCA() {
	uchar temp = (CPU.A & 0x1);
	Flag = temp << 4;
	CPU.A = CPU.A >> 1;
	CPU.A |= (temp << 7);
	/*if (CPU.A == 0)
		Flag |= 0x80;*/
	CPU.CPUTicks += 4;
}

void SLA(uchar * A){
	Flag = (*A >> 7) << 4;
	*A = *A << 1;
	if (*A == 0)
		Flag |= 0x80;
	CPU.CPUTicks += 4;
}

void SRA(uchar * A){
	uchar temp = (*A & 0x80);//old msb
	Flag = (*A & 0x1) << 4;
	*A = *A >> 1;
	*A |= temp;
	if (*A == 0)
		Flag |= 0x80;
	CPU.CPUTicks += 4;
}

void SRL(uchar * A){
	Flag = (*A & 0x1) << 4;
	*A = *A >> 1;
	if (*A == 0)
		Flag |= 0x80;
	CPU.CPUTicks += 4;
}

void BIT(uchar A, uchar B) {
	Flag |= 0x20;
	Flag &= ~0x40;
	if (((A >> B) & 0x1) == 0)
		Flag |= 0x80;
	else
		Flag &= ~0x80;
	CPU.CPUTicks += 4;
}

void SWAP(uchar * A){
	uchar h = *A & 0XF0;
	uchar l = *A & 0X0F;
	*A = (h >> 4) | (l << 4);
	if (*A == 0)
		Flag = 0x80;
	else
		Flag = 0;
	CPU.CPUTicks += 4;
}

void RES(uchar *A,uchar B){
	*A &= ~(0x1 << B);
	CPU.CPUTicks += 4;
}

void SET(uchar *A,uchar B){
	*A |= (0x1 << B);
	CPU.CPUTicks += 4;
}
