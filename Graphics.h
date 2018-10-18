#pragma once

#include "defs.h"

#ifndef gpuh
#define gpuh

void initGraphics();
void exitGraphics();
void printSprites();
void printVTable();
void printSTable();
void printMemory();
void NextGraphic();
void printPixel();
void printBuffer();
void flipSpriteN(uchar tile, uchar b, uchar x1, uchar y1);
void flipSpriteV(uchar tile, uchar b, uchar x1, uchar y1);
void flipSpriteH(uchar tile, uchar b, uchar x1, uchar y1);
void flipSpriteVH(uchar tile, uchar b, uchar x1, uchar y1);
#endif