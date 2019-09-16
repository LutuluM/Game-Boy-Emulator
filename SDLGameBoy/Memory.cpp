#include "Memory.h"
#include "CPU.h"
#include "IOs.h"
#include "sound.h"


///Loaded Roms
uchar * Brom;
uchar * GameCode;


///Memory Map
uchar * Rom0;
uchar * Rom1;
uchar * Vram;
uchar * CartRam;
uchar * CartRamBank;
uchar * Wram0;
uchar * Wram1;
uchar * WramBank;//only swappable when in cgb mode. so not used
uchar * WramEcho;
uchar SAtable[0xa0]{ 0 };
uchar notused[0x60]{ 0 };
uchar IO[0x80] = {0};
uchar Hram[0x7f]{ 0 };
uchar Interrupt[1]{ 0 };

///Other Variables
uchar Ramenable;
uchar Booting;

uchar upperBitsMBC1 = 0;
uchar ROMRAMMBC1 = 0;
uchar RTC = 0;
ushort totalBanks;
char * saveName;



void changeROM1(uchar data)
{
	const uchar RomSize = Rom0[0x148];
	data &= 0x1F;//get lower 5 bits
	//printf("data = %.2X", data);
	if (ROMRAMMBC1 == 0)
		data |= (upperBitsMBC1 << 5);//if rom banking then get upper 2 for banks
	if ((data & 0x1F) == 0) //lower bit is zero
	{
		printf("data = %.2X", data);
		data &= 0xFF >> (7 - RomSize);//max number of banks
		printf("->%.2X  -- ", data);
		Rom1 = &GameCode[(++data) * 0x4000];//bank 0 is not referenced by rom1 directly
		printf("ROM changed to Bank %X\n", data);
		printf("Rom Size is %X", RomSize);
	}
	else
	{
		data %= (0xFF >> (7 - RomSize))+1;//max number of banks
		//printf("->%.2X  -- ", data);
		Rom1 = &GameCode[data * 0x4000];
	}
	//printf("ROM changed to Bank %d\n", data);
	/*const uchar RomSize = Rom0[0x148];
	data &= 0x1F;//get lower 5 bits
	printf("data = %.2X", data);
	if (ROMRAMMBC1 == 0)
		data |= (upperBitsMBC1 << 5);//if rom banking then get upper 2 for banks
	data &= 0xFF >> (7 - RomSize);//max number of banks
	printf("->%.2X  -- ", data);
	if ((data & 0x1F) == 0)
		Rom1 = &GameCode[(++data)*0x4000];//bank 0 is never referenced by rom1
	else
		Rom1 = &GameCode[data * 0x4000];
	printf("ROM changed to Bank %d\n", data);*/
}

void changeRAM1(uchar data) {
	const uchar RamSize = Rom0[0x149];
	data &= 0x03;
	if (ROMRAMMBC1)
	{
		if (RamSize == 0x2)
		{
			printf("2K RAM");
			CartRam = &CartRamBank[0];
		}
			
		else
			CartRam = &CartRamBank[data * 0x2000];
	}
	else
	{
		CartRam = &CartRamBank[0x0];//set ram to bank 1
		upperBitsMBC1 = data;
	}
}

void changeROM3(uchar data)
{
	data &= 0x7F;
	if (data == 0)
		Rom1 = &GameCode[0x4000];
	else
		Rom1 = &GameCode[data * 0x4000];
	if (data > totalBanks)
		printf("ROM CHANGED TO ILLEGAL ADDRESS %d|0x%X",data,data);
	//printf("ROM changed to Bank %d\n", data);
}

void changeRAM3(uchar data) {
	if (data > 0x7 && data < 0xD)
	{
		RTC = 1;
		printf("RTC Clock Selected\n");//update so that there are 5 registers
		return;//RTC Mapped to memory
	}
	
	data &= 0x03;
	CartRam = &CartRamBank[data * 0x2000];
	printf("CartRam changed to Bank %d\n", data);
}

void MBC1(ushort location, uchar data)
{
	printf("MBC1 \n");
	if (location <= 0x1FFF)
	{
		if ((data & 0x0F) == 0x0A)
			Ramenable = 1;
		else
			Ramenable = 0;
	}
	else if (location <= 0x3FFF)
		changeROM1(data);
	else if (location <= 0x5FFF)
		changeRAM1(data);
	else
	{
		if (data & 0x1) {
			ROMRAMMBC1 = 1;
			printf("RAM Banking Mode\n");
		}
		else
		{
			ROMRAMMBC1 = 0;
			printf("R0M Banking Mode\n");
		}
	}
}

void MBC2(ushort location, uchar data)
{
	printf("MBC2 \n");
}

void MBC3(ushort location, uchar data)
{
	//printf("MBC3 \n");
	if (location <= 0x1FFF)
	{
		if ((data & 0x0F) == 0x0A)
			Ramenable = 1;
		else
			Ramenable = 0;
	}
	else if (location <= 0x3FFF)
		changeROM3(data);
	else if (location <= 0x5FFF)
		changeRAM3(data);
	else
	{
		printf("Real Time Clock Latch 0x%X\n",data);
	}
}

void writeMem(ushort location, uchar data) {
	static const uchar CartType = Rom0[0x147];//Memory scheme for cartridge
	if (location <= 0x7FFF)
	{
		if (CartType == 0)
			printf("Write when Rom only\n");
		else if (CartType < 0x4)
			MBC1(location, data);
		else if (CartType < 0x7)
			MBC2(location, data);
		else if (CartType < 0x14)
			MBC3(location, data);
	}

	else if (location <= 0x9FFF)
		Vram[location - 0x8000] = data;

	else if (location <= 0xBFFF)
	{
		if (Ramenable)
			CartRam[location - 0xA000] = data;
	}

	else if (location <= 0xCFFF)
		Wram0[location - 0xC000] = data;

	else if (location <= 0xDFFF)
		Wram1[location - 0xD000] = data;

	else if (location <= 0xFDFF)
		WramEcho[location - 0xE000] = data;

	else if (location <= 0xFE9F)
		SAtable[location - 0xFE00] = data;

	else if (location <= 0xFEFF)
	{
		//printf("Writing to The Not Used Address!!!! %X\n",getPC());
		//notused[location - 0xFEA0] = data;
		//do nothing
	}

	else if (location <= 0xFF7F)//IO port register edits
	{	/*Make this a a switch statment*/
		switch(location)
		{
		case 0xFF00:
			IO[0] = (data & 0xF0) | (IO[0] & 0x0F);
			break;
		case 0xFF02:
			if ((data & 0x80) == 0x80)
				writeMem(0xFF0F, readMem(0xFF0F) & 0x08);//set flag since serial isn't planned
			IO[0x02] = data;
			break;
		case 0xFF04:
			IO[0x04] = 0;
			divReset();
			break;

		case 0xFF10:
			square1->soundOnReg(data);
			break;
		case 0xFF11:
			square1->soundLengthReg(data);
			break;
		case 0xFF12:
			square1->soundVolReg(data);
			break;
		case 0xFF13:
			square1->soundFreqLowReg(data);
			break;
		case 0xFF14:
			square1->soundFreqHiReg(data);
			break;

		case 0xFF15:
			square2->soundOnReg(data);
			break;
		case 0xFF16:
			square2->soundLengthReg(data);
			break;
		case 0xFF17:
			square2->soundVolReg(data);
			break;
		case 0xFF18:
			square2->soundFreqLowReg(data);
			break;
		case 0xFF19:
			square2->soundFreqHiReg(data);
			break;

		case 0xFF1A:
			sine->soundOnReg(data);
			break;
		case 0xFF1B:
			sine->soundLengthReg(data);
			break;
		case 0xFF1C:
			sine->soundVolReg(data);
			break;
		case 0xFF1D:
			sine->soundFreqLowReg(data);
			break;
		case 0xFF1E:
			sine->soundFreqHiReg(data);
			break;

		case 0xFF1F:
			noise->soundOnReg(data);
			break;
		case 0xFF20:
			noise->soundLengthReg(data);
			break;
		case 0xFF21:
			noise->soundVolReg(data);
			break;
		case 0xFF22:
			noise->soundPolyReg(data);
			break;
		case 0xFF23:
			noise->soundCounterReg(data);
			break;

		case 0xFF26:
			masterSoundEnable(data);
			break;

		case 0xFF41:
			IO[0x41] = (data & 0xF8) | (IO[0x41] & 0x07);
			break;
		case 0xFF46:
			DMA(data);
			break;
		case 0xFF50:
			Booting = 0;
			IO[0x50] = data;
			break;

		default:
			IO[location - 0xFF00] = data;
			break;
		}			
	}
	else if (location <= 0xFFFE)
		Hram[location - 0xFF80] = data;

	else if (location == 0xFFFF)
		Interrupt[location - 0xFFFF] = data;
	else
		printf("Memory Write Location Error");
}

uchar readMem(ushort location) {
	if (location <= 0x3FFF) {
		if (Booting && (location < 0x100))//only needed in Boot Rom
			return Brom[location];
		return Rom0[location];
	}
	else if (location <= 0x7FFF)
		return Rom1[location - 0x4000];

	else if (location <= 0x9FFF)
		return Vram[location - 0x8000];

	else if (location <= 0xBFFF)
	{
		if (Ramenable)
		{
			if (RTC)
			{
				printf("Real Time Clock was read from @ 0x%X",location);
				return 0x0;//just a random number at this time
			}
			else
				return CartRam[location - 0xA000];
		}
		printf("Ram Disabled\n");
		return 0xFF;
	}

	else if (location <= 0xCFFF)
		return Wram0[location - 0xC000];

	else if (location <= 0xDFFF)
		return Wram1[location - 0xD000];

	else if (location <= 0xFDFF)
		return WramEcho[location - 0xE000];

	else if (location <= 0xFE9F)
		return SAtable[location - 0xFE00];

	else if (location <= 0xFEFF)
	{
		printf("Reading unused\n");
		return 0xFF;//notused[location - 0xFEA0];
	}

	else if (location <= 0xFF7F)
		return IO[location - 0xFF00];

	else if (location <= 0xFFFE)
		return Hram[location - 0xFF80];

	else if (location == 0xFFFF)
		return Interrupt[location - 0xFFFF];
	printf("ERROR READ OUT OF BOUNDS!");
	getchar();
	exit(0);
	return 0;//never reached
}

void dumpMem(ushort start, ushort bytes)
{
	FILE * file;
	file = fopen("Dump.txt", "w");
	for (ushort i = start; i < start + bytes; i++)
	{
		fprintf(file, "0x%.4X: 0x%.2X\n", i, readMem(i));
	}
	fprintf(file, "==CPU REGISTERS==\n");
	fprintf(file, "AF: 0x%.4X\n", getReg(0));
	fprintf(file, "BC: 0x%.4X\n", getReg(1));
	fprintf(file, "DE: 0x%.4X\n", getReg(2));
	fprintf(file, "HL: 0x%.4X\n", getReg(3));
	fprintf(file, "PC: 0x%.4X\n", getPC());
	fprintf(file, "SP: 0x%.4X\n", getSP());
	fprintf(file, "Interrupt Enable: 0x%.2X\n", readMem(0xFFFF));
	fprintf(file, "Interrupt Flags: 0x%.2X\n", readMem(0xFF0F));
	fprintf(file, "Interrupt Master: %d\n", getIME());

	fclose(file);
}

void initGameMem() {
	CartRamBank = (uchar *)malloc(0x8000);
	Wram0 = (uchar *)malloc(0x1000);
	WramBank = (uchar *)malloc(0x8000);
}

void initMem() {
	Vram = (uchar *) malloc(0x2000);
	Rom0 = &GameCode[0];
	Rom1 = &GameCode[0x4000];
	CartRam = &CartRamBank[0];
	Wram1 = &WramBank[0x1000];
	WramEcho = &Wram0[0];
	Ramenable = 0;
	Booting = 1;
	setJoy(0x3F); //clear buttons
}

void setJoy(uchar A) {
	IO[0] = A;
}

void setLCDC(uchar A) {
	IO[0x41] = A;
}

void setDivTimer(uchar A) {
	IO[0x04] = A;
}

uchar * loadROM(char * Romname) {
	int lSize;
	FILE *fp;

	fp = fopen(Romname, "rb");

	if (fp == 0) {
		printf("failed to load ROM.");
		exit(-1);
	}

	fseek(fp, 0, SEEK_END);
	lSize = ftell(fp);
	rewind(fp);

	uchar * temp = (uchar *) malloc(lSize);
	
	fread(temp, 1, lSize, fp);
	fclose(fp);
	totalBanks = lSize / 0x4000;
	printf("Rom:%s is 0x%X RomBanks %d|0x%X\n",Romname,lSize,totalBanks,totalBanks);
	return temp;
}

void loadBIOS(char * Romname) {
	int lSize;
	FILE *fp;

	fp = fopen(Romname, "rb");

	if (fp == 0)
	{
		printf("BootRom \"%s\" not found",Romname);
		exit(-1);
	}
		

	fseek(fp, 0, SEEK_END);
	lSize = ftell(fp);
	rewind(fp);

	Brom = (uchar *) malloc(lSize);
	
	fread(Brom, 1, lSize, fp);
	fclose(fp);
	printf("Boot Rom:%s is 0x%X\n",Romname,lSize);
}

void loadGAME(char * Romname) {
	int lSize;
	FILE *fp;

	fp = fopen(Romname, "rb");

	if (fp == 0)
	{
		printf("Game ROM \"%s\" not found", Romname);
		exit(-1);
	}
		

	fseek(fp, 0, SEEK_END);
	lSize = ftell(fp);
	rewind(fp);

	GameCode = (uchar *) malloc(lSize);
	
	fread(GameCode, 1, lSize, fp);
	fclose(fp);
	totalBanks = lSize / 0x4000;
	printf("Rom:%s is 0x%X RomBanks %d|0x%X\n",Romname,lSize,totalBanks,totalBanks);
	loadSave(Romname);
}

void loadSave(char * Romname) {
	int x = 0;
	while (Romname[x] != '\0')
		x++;//x is null char index
	saveName = (char *) malloc(x + 1);//1 for keeping the null char
	x = -1;
	do {
		x++;
		*(saveName + x) = *(Romname + x);
	} while (saveName[x] != '.');
	saveName[++x] = 's';
	saveName[++x] = 'a';
	saveName[++x] = 'v';
	saveName[++x] = '\0';

	int lSize;
	FILE *fp;
	
	fp = fopen(saveName, "rb");

	if (fp == 0)
	{
		//fclose(fp);
		printf("No save Found.\n");
		return;//no save file
	}

	fseek(fp, 0, SEEK_END);
	lSize = ftell(fp);
	rewind(fp);
	
	fread(CartRamBank, 1, lSize, fp);
	fclose(fp);
}

void saveGame() {
	FILE *fp;

	fp = fopen(saveName, "wb");

	if (fp == 0){
		printf("failed to create save.");
		exit(-1);
	}
	fwrite(CartRamBank, 1, 0x8000, fp);
	fclose(fp);
}

void exitMem() {
	free(Brom);
	free(GameCode);
	//free(saveName);
}

void soundMemWrite(uchar adr,uchar value) {
	IO[adr] = value;
}