#ifndef IOs
#define IOs

extern uchar running;

void checkIO();
void keyboard();
void Interrupts();
void Timer();
void LCDC();
void DMA(ushort adr);
void divReset();
#endif // !1

