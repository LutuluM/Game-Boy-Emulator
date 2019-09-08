#include "defs.h"

#ifndef gpuh
#define gpuh

void initGraphics();
void exitGraphics();

void NextGraphic();
void printBackGroundTiles();
void printWindowTiles();
void printBuffer();
void printFrame();

void printSprites();
void flipSpriteN(uchar tile, uchar colorPalette, uchar x1, uchar y1, uchar prio);
void flipSpriteV(uchar tile, uchar colorPalette, uchar x1, uchar y1, uchar prio);
void flipSpriteH(uchar tile, uchar colorPalette, uchar x1, uchar y1, uchar prio);
void flipSpriteVH(uchar tile, uchar colorPalette, uchar x1, uchar y1, uchar prio);

void printMemory();
void printVTable();
void printSTable();

uchar getSL();
uchar getfps();
#endif