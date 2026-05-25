#include "start_screen.h"
#include "raylib.h"


static Texture2D telaInicial;
static Rectangle btnPlay = { 960.0f/2.0f - 130.0f, 720.0f, 500.0f, 80.0f };
static bool mouseNoBotao = false;


static bool iniciandoTransicao = false;
static float fadeAlpha = 0.0f;  

void InitStartScreen(void) {
    telaInicial = LoadTexture("Tela inicial.png");
}

void UnloadStartScreen(void) {
    UnloadTexture(telaInicial);
}

TelaAtual UpdateStartScreen(void) {

    if (iniciandoTransicao) {
        fadeAlpha += 0.008f; 
        

        if (fadeAlpha >= 1.0f) {
            fadeAlpha = 0.0f; 
            iniciandoTransicao = false;
            return TELA_INTRO; 
        }
        
        return TELA_START; 
    }
    
 
    Vector2 mousePoint = GetMousePosition();
    
    if (CheckCollisionPointRec(mousePoint, btnPlay)) {
        mouseNoBotao = true;
    
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            iniciandoTransicao = true; 
        }
    } else {
        mouseNoBotao = false;
    }
    
    return TELA_START;
}

void DrawStartScreen(void) {
    ClearBackground(BLACK);

    DrawTexturePro(telaInicial,
        (Rectangle){ 0, 0, (float)telaInicial.width, (float)telaInicial.height },
        (Rectangle){ 0, 0, 1920, 1080 },
        (Vector2){ 0, 0 }, 0.0f, WHITE);
    
 
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
             

    if (iniciandoTransicao) {
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, fadeAlpha));
    }
}