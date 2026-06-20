#include "intro.h"
#include "fonte.h"
#include "raylib.h"

static Texture2D telaIntro;
static Texture2D spriteAndandoDireita;
static Texture2D spriteAndandoEsquerda;
static Texture2D spriteParadoDireita;
static Texture2D spriteParadoEsquerda;

static bool iniciandoTransicao = false;
static float fadeAlpha = 0.0f;

static bool lendoDialogo      = true;
static bool posicionouNaPonte = false;

static float animTimer     = 0.0f;
static bool  frameAndando  = false;
static bool  virandoDireita = true;
static bool  movendo        = false;

void InitIntro(void) {
    telaIntro             = LoadTexture("assets/images/universal/tela_intro.png");
    spriteAndandoDireita  = LoadTexture("assets/images/zelezinho/Zelezinho andando direita.png");
    spriteAndandoEsquerda = LoadTexture("assets/images/zelezinho/Zelezinho andando esquerda.png");
    spriteParadoDireita   = LoadTexture("assets/images/zelezinho/Zelezinho parado direita.png");
    spriteParadoEsquerda  = LoadTexture("assets/images/zelezinho/Zelezinho parado esquerda.png");
}

void UnloadIntro(void) {
    UnloadTexture(telaIntro);
    UnloadTexture(spriteAndandoDireita);
    UnloadTexture(spriteAndandoEsquerda);
    UnloadTexture(spriteParadoDireita);
    UnloadTexture(spriteParadoEsquerda);
}

TelaAtual UpdateIntro(Personagem *player) {
    if (!posicionouNaPonte) {
        player->posicao.x = 5.0f;
        player->posicao.y = 970.0f;
        posicionouNaPonte = true;
    }

    if (lendoDialogo) {
        if (IsKeyPressed(KEY_ENTER)) lendoDialogo = false;
        return TELA_INTRO;
    }

    if (iniciandoTransicao) {
        fadeAlpha += 0.01f;
        if (fadeAlpha >= 1.0f) {
            fadeAlpha         = 0.0f;
            iniciandoTransicao = false;
            lendoDialogo      = true;
            posicionouNaPonte = false;
            frameAndando      = false;
            animTimer         = 0.0f;
            player->posicao.x = 100.0f;
            player->posicao.y = 900.0f;
            return TELA_ENTRADA;
        }
        return TELA_INTRO;
    }

    float velocidade   = 5.0f;
    bool  pressionouD  = IsKeyDown(KEY_D);
    bool  pressionouA  = IsKeyDown(KEY_A);

    if (pressionouD) { player->posicao.x += velocidade; virandoDireita = true; }
    if (pressionouA) { player->posicao.x -= velocidade; virandoDireita = false; }

    movendo = pressionouD || pressionouA;

    if (movendo) {
        animTimer += GetFrameTime();
        if (animTimer >= 0.18f) { animTimer = 0.0f; frameAndando = !frameAndando; }
    } else {
        frameAndando = false;
        animTimer    = 0.0f;
    }

    if (player->posicao.x < 5.0f)     player->posicao.x = 5.0f;
    if (player->posicao.x >= 1900.0f) iniciandoTransicao = true;

    return TELA_INTRO;
}

static void DesenharZelezinho(Personagem player, bool virandoDireitaLocal, bool frameAndandoLocal) {
    Texture2D sprite = virandoDireitaLocal
        ? (frameAndandoLocal ? spriteAndandoDireita  : spriteParadoDireita)
        : (frameAndandoLocal ? spriteAndandoEsquerda : spriteParadoEsquerda);

    Rectangle src  = { 0, 0, 1254, 1254 };
    float     disp = 500.0f;
    Rectangle dest = {
        player.posicao.x - disp * 0.5f,
        player.posicao.y - disp * 0.70f,
        disp, disp
    };
    DrawTexturePro(sprite, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
}

void DrawIntro(Personagem player) {
    ClearBackground(BLACK);

    DrawTexturePro(telaIntro,
        (Rectangle){ 0, 0, (float)telaIntro.width, (float)telaIntro.height },
        (Rectangle){ 0, 0, 1920, 1080 },
        (Vector2){ 0, 0 }, 0.0f, WHITE);

    DesenharZelezinho(player, virandoDireita, frameAndando);

    if (lendoDialogo) {
        Rectangle caixaDialogo = { 100.0f, 750.0f, 1720.0f, 200.0f };
        DrawRectangleRec(caixaDialogo, DARKGRAY);
        DrawRectangleLinesEx(caixaDialogo, 5.0f, BLACK);
        DrawTextF("Voce:", 130, 780, 30, SKYBLUE);
        DrawTextF("\"Finalmente consegui um emprego... acho que dessa vez vai!\"", 130, 850, 40, WHITE);
        DrawTextF("[PRESSIONE ENTER PARA CONTINUAR]", 1350, 910, 20, LIGHTGRAY);
    }

    if (iniciandoTransicao) {
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, fadeAlpha));
    }
}
