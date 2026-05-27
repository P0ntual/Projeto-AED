#include "fonte.h"
#include <stddef.h>

static Font fonte;

void InitFonte(void) {
    fonte = LoadFontEx("assets/fonts/ShadowsIntoLight-Regular.ttf", 128, NULL, 0);
    SetTextureFilter(fonte.texture, TEXTURE_FILTER_BILINEAR);
}

void UnloadFonte(void) {
    UnloadFont(fonte);
}

void DrawTextF(const char *text, int x, int y, int size, Color color) {
    DrawTextEx(fonte, text, (Vector2){(float)x, (float)y}, (float)size * 2.0f, 1.0f, color);
}

int MeasureTextF(const char *text, int size) {
    return (int)MeasureTextEx(fonte, text, (float)size * 2.0f, 1.0f).x;
}
