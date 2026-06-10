#include "entrada.h"
#include "raylib.h"

static Texture2D telaEntrada;
static Texture2D spriteAndandoDireita;
static Texture2D spriteAndandoEsquerda;
static Texture2D spriteParadoDireita;
static Texture2D spriteParadoEsquerda;

static Rectangle portaEntrada = { 500, 800, 920, 200 };

static bool iniciandoTransicao = false;
static float fadeAlpha = 0.0f;

static float animTimer      = 0.0f;
static bool  frameAndando   = false;
static bool  virandoDireita = true;

static bool  entrouNaEntrada = false;
static float yInicial        = 0.0f;

void InitEntrada(void) {
    telaEntrada           = LoadTexture("assets/images/universal/tela_frente.png");
    spriteAndandoDireita  = LoadTexture("assets/images/zelezinho/Zelezinho andando direita.png");
    spriteAndandoEsquerda = LoadTexture("assets/images/zelezinho/Zelezinho andando esquerda.png");
    spriteParadoDireita   = LoadTexture("assets/images/zelezinho/Zelezinho parado direita.png");
    spriteParadoEsquerda  = LoadTexture("assets/images/zelezinho/Zelezinho parado esquerda.png");
}

void UnloadEntrada(void) {
    UnloadTexture(telaEntrada);
    UnloadTexture(spriteAndandoDireita);
    UnloadTexture(spriteAndandoEsquerda);
    UnloadTexture(spriteParadoDireita);
    UnloadTexture(spriteParadoEsquerda);
}

TelaAtual UpdateEntrada(Personagem *player) {
    float raio = 20.0f;

    if (!entrouNaEntrada) {
        yInicial = player->posicao.y;
        entrouNaEntrada = true;
    }
    player->posicao.y = yInicial;

    if (player->posicao.x < raio)           player->posicao.x = raio;
    if (player->posicao.x > 1920.0f - raio) player->posicao.x = 1920.0f - raio;

    bool pressionouD = IsKeyDown(KEY_D);
    bool pressionouA = IsKeyDown(KEY_A);
    bool pressionouW = IsKeyDown(KEY_W);
    bool pressionouS = IsKeyDown(KEY_S);
    bool movendo     = pressionouD || pressionouA || pressionouW || pressionouS;

    if (pressionouD) virandoDireita = true;
    if (pressionouA) virandoDireita = false;

    if (movendo) {
        animTimer += GetFrameTime();
        if (animTimer >= 0.18f) { animTimer = 0.0f; frameAndando = !frameAndando; }
    } else {
        frameAndando = false;
        animTimer    = 0.0f;
    }

    if (iniciandoTransicao) {
        fadeAlpha += 0.02f;
        if (fadeAlpha >= 1.0f) {
            fadeAlpha          = 0.0f;
            iniciandoTransicao = false;
            frameAndando       = false;
            animTimer          = 0.0f;
            entrouNaEntrada    = false;
            player->posicao.x  = 960.0f;
            player->posicao.y  = 700.0f;
            return TELA_GAMEPLAY;
        }
        return TELA_ENTRADA;
    }

    if (CheckCollisionCircleRec(player->posicao, raio, portaEntrada)) {
        iniciandoTransicao = true;
    }

    return TELA_ENTRADA;
}

void DrawEntrada(Personagem player) {
    ClearBackground(BLACK);

    DrawTexturePro(telaEntrada,
        (Rectangle){ 0, 0, (float)telaEntrada.width, (float)telaEntrada.height },
        (Rectangle){ 0, 0, 1920, 1080 },
        (Vector2){ 0, 0 }, 0.0f, WHITE);

    Texture2D sprite = virandoDireita
        ? (frameAndando ? spriteAndandoDireita  : spriteParadoDireita)
        : (frameAndando ? spriteAndandoEsquerda : spriteParadoEsquerda);

    Rectangle src  = { 0, 0, 1254, 1254 };
    float     disp = 500.0f;
    Rectangle dest = {
        player.posicao.x - disp * 0.5f,
        player.posicao.y - disp * 0.50f,
        disp, disp
    };
    DrawTexturePro(sprite, src, dest, (Vector2){0, 0}, 0.0f, WHITE);

    if (iniciandoTransicao) {
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, fadeAlpha));
    }
}
