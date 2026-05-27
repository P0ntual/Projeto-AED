#include "start_screen.h"
#include "fonte.h"
#include "raylib.h"


static Texture2D telaInicial;
static Texture2D btnTexture;
static Rectangle btnPlay    = { 960.0f/2.0f - 130.0f, 720.0f, 500.0f, 80.0f };
static Rectangle btnRanking = { 960.0f/2.0f - 130.0f, 820.0f, 500.0f, 70.0f };
static bool mouseNoBotaoPlay    = false;
static bool mouseNoBotaoRanking = false;


static bool iniciandoTransicao = false;
static float fadeAlpha = 0.0f;

void InitStartScreen(void) {
    telaInicial = LoadTexture("assets/images/tela_inicial.png");
    btnTexture  = LoadTexture("assets/images/botao].png");
}

void UnloadStartScreen(void) {
    UnloadTexture(telaInicial);
    UnloadTexture(btnTexture);
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
    
 
    Rectangle btnSrc = { 25, 239, 757, 279 };
    Color tintPlay    = mouseNoBotaoPlay    ? LIGHTGRAY : WHITE;
    Color tintRanking = mouseNoBotaoRanking ? LIGHTGRAY : WHITE;
    int sz = 20;

    DrawTexturePro(btnTexture, btnSrc, btnPlay, (Vector2){0,0}, 0.0f, tintPlay);
    int larguraPlay = MeasureTextF("PLAY", sz);
    DrawTextF("PLAY",
             (int)(btnPlay.x + btnPlay.width/2  - larguraPlay/2),
             (int)(btnPlay.y + btnPlay.height/2 - sz),
             sz, WHITE);

    DrawTexturePro(btnTexture, btnSrc, btnRanking, (Vector2){0,0}, 0.0f, tintRanking);
    int larguraRanking = MeasureTextF("RANKING", sz);
    DrawTextF("RANKING",
             (int)(btnRanking.x + btnRanking.width/2  - larguraRanking/2),
             (int)(btnRanking.y + btnRanking.height/2 - sz),
             sz, WHITE);


    if (iniciandoTransicao) {
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, fadeAlpha));
    }
}