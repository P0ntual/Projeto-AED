#include "start_screen.h"
#include "raylib.h"


static Texture2D telaInicial;
static Rectangle btnPlay    = { 960.0f/2.0f - 130.0f, 720.0f, 500.0f, 80.0f };
static Rectangle btnRanking = { 960.0f/2.0f - 130.0f, 820.0f, 500.0f, 70.0f };
static bool mouseNoBotaoPlay    = false;
static bool mouseNoBotaoRanking = false;


static bool iniciandoTransicao = false;
static float fadeAlpha = 0.0f;

void InitStartScreen(void) {
    telaInicial = LoadTexture("assets/images/tela_inicial.png");
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

    mouseNoBotaoPlay    = CheckCollisionPointRec(mousePoint, btnPlay);
    mouseNoBotaoRanking = CheckCollisionPointRec(mousePoint, btnRanking);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (mouseNoBotaoPlay) {
            iniciandoTransicao = true;
        } else if (mouseNoBotaoRanking) {
            return TELA_RANKING;
        }
    }

    return TELA_START;
}

void DrawStartScreen(void) {
    ClearBackground(BLACK);

    DrawTexturePro(telaInicial,
        (Rectangle){ 0, 0, (float)telaInicial.width, (float)telaInicial.height },
        (Rectangle){ 0, 0, 1920, 1080 },
        (Vector2){ 0, 0 }, 0.0f, WHITE);
    
 
    DrawRectangleRec(btnPlay, mouseNoBotaoPlay ? GREEN : DARKGREEN);
    int tamanhoTexto = 40;
    int larguraPlay = MeasureText("PLAY", tamanhoTexto);
    DrawText("PLAY",
             (int)(btnPlay.x + btnPlay.width/2 - larguraPlay/2),
             (int)(btnPlay.y + btnPlay.height/2 - tamanhoTexto/2),
             tamanhoTexto,
             BLACK);

    DrawRectangleRec(btnRanking, mouseNoBotaoRanking ? GOLD : (Color){ 150, 110, 30, 255 });
    int tamanhoRanking = 32;
    int larguraRanking = MeasureText("RANKING", tamanhoRanking);
    DrawText("RANKING",
             (int)(btnRanking.x + btnRanking.width/2 - larguraRanking/2),
             (int)(btnRanking.y + btnRanking.height/2 - tamanhoRanking/2),
             tamanhoRanking,
             BLACK);


    if (iniciandoTransicao) {
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, fadeAlpha));
    }
}