#ifndef TEXT_H
#define TEXT_H

#include "display.h"
#include <math.h>

void drawHeader(int x, int y, char *text);
void drawText(int x, int y, char *text);
void drawTextCentered(char *text, int _y);
void drawWave(int y, float speed, float amplitude);

#endif