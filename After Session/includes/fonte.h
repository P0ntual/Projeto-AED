#ifndef FONTE_H
#define FONTE_H

#include "raylib.h"

void InitFonte(void);
void UnloadFonte(void);
void DrawTextF(const char *text, int x, int y, int size, Color color);
int  MeasureTextF(const char *text, int size);

#endif
