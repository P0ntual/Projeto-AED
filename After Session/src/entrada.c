#include "entrada.h"
#include "raylib.h"

static Texture2D telaEntrada;
static Rectangle portaEntrada = { 500, 800, 920, 200 };

static bool iniciandoTransicao = false;
static float fadeAlpha = 0.0f;

void InitEntrada(void) {
    telaEntrada = LoadTexture("assets/images/tela_frente.png");
}

void UnloadEntrada(void) {
    UnloadTexture(telaEntrada);
}

TelaAtual UpdateEntrada(Personagem *player) {
    float raio = 20.0f;

    if (player->posicao.x < raio) player->posicao.x = raio;
    if (player->posicao.x > 1920.0f - raio) player->posicao.x = 1920.0f - raio;
    if (player->posicao.y < raio) player->posicao.y = raio;
    if (player->posicao.y > 1080.0f - raio) player->posicao.y = 1080.0f - raio;

    if (iniciandoTransicao) {
        fadeAlpha += 0.02f;
        if (fadeAlpha >= 1.0f) {
            fadeAlpha = 0.0f;
            iniciandoTransicao = false;
            player->posicao.x = 960.0f;
            player->posicao.y = 700.0f;
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

    DrawPersonagem(player);

    if (iniciandoTransicao) {
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, fadeAlpha));
    }
}
