#include "start_screen.h"
#include "raylib.h"

static Rectangle btnPlay = {1080.0f/3.0f - 150.0f, 100.0f, 500.0f, 80.0f };

static bool mouseNoBotao = false;
TelaAtual UpdateStartScreen(void) {
    Vector2 mousePoint = GetMousePosition();
    if (CheckCollisionPointRec(mousePoint, btnPlay)) {
        mouseNoBotao = true;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            return TELA_INTRO;
        }
    } else {
        mouseNoBotao = false;
    }
    return TELA_START;
}
void DrawStartScreen(void) {
    ClearBackground(BLACK);
    Color corDoBotao;
    if (mouseNoBotao) {
        corDoBotao = GREEN;
    } else {
        corDoBotao = DARKGREEN;
    }
    DrawRectangleRec(btnPlay, corDoBotao);
    int tamanhoTexto = 40;
    int larguraTexto = MeasureText("PLAY", tamanhoTexto);
    DrawText("PLAY", 
             (int)(btnPlay.x + btnPlay.width/2 - larguraTexto/2), 
             (int)(btnPlay.y + btnPlay.height/2 - tamanhoTexto/2),
             tamanhoTexto, 
             BLACK);
}

