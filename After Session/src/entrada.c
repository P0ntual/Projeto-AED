#include "entrada.h"
#include "raylib.h"

static Rectangle portaEntrada = { 700, 720, 520, 80 };

static bool iniciandoTransicao = false;
static float fadeAlpha = 0.0f;

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
    // Ceu
    ClearBackground((Color){ 100, 149, 237, 255 });

    // Fachada principal (creme)
    DrawRectangle(200, 120, 1520, 810, (Color){ 245, 235, 200, 255 });

    // Letreiro vermelho
    DrawRectangle(420, 108, 1080, 150, MAROON);
    DrawText("CINEMA SAO LUIZ", 480, 138, 85, WHITE);

    // Friso decorativo abaixo do letreiro
    DrawRectangle(420, 258, 1080, 20, (Color){ 180, 160, 120, 255 });

    // Coluna esquerda
    DrawRectangle(510, 278, 100, 640, (Color){ 210, 200, 175, 255 });
    DrawRectangle(500, 258, 120, 20, (Color){ 190, 180, 155, 255 });

    // Coluna direita
    DrawRectangle(1310, 278, 100, 640, (Color){ 210, 200, 175, 255 });
    DrawRectangle(1300, 258, 120, 20, (Color){ 190, 180, 155, 255 });

    // Moldura das portas (marrom escuro)
    DrawRectangle(700, 280, 520, 650, (Color){ 100, 70, 20, 255 });

    // Porta esquerda (dourada)
    DrawRectangle(713, 293, 234, 629, (Color){ 218, 165, 32, 255 });

    // Porta direita (dourada)
    DrawRectangle(955, 293, 253, 629, (Color){ 218, 165, 32, 255 });

    // Divisor central
    DrawRectangle(945, 293, 14, 629, (Color){ 100, 70, 20, 255 });

    // Detalhes decorativos nas portas
    DrawRectangle(730, 350, 200, 10, (Color){ 180, 135, 20, 255 });
    DrawRectangle(730, 560, 200, 10, (Color){ 180, 135, 20, 255 });
    DrawRectangle(970, 350, 220, 10, (Color){ 180, 135, 20, 255 });
    DrawRectangle(970, 560, 220, 10, (Color){ 180, 135, 20, 255 });

    // Macanetas
    DrawCircle(944, 620, 10, (Color){ 80, 55, 10, 255 });
    DrawCircle(960, 620, 10, (Color){ 80, 55, 10, 255 });

    // Degraus
    DrawRectangle(640, 930, 640, 26, (Color){ 195, 190, 180, 255 });
    DrawRectangle(600, 956, 720, 26, (Color){ 185, 180, 170, 255 });
    DrawRectangle(560, 982, 800, 26, (Color){ 175, 170, 160, 255 });

    // Calcada
    DrawRectangle(0, 1008, 1920, 72, (Color){ 130, 130, 130, 255 });

    // Linhas da calcada
    for (int i = 0; i < 1920; i += 120) {
        DrawRectangle(i, 1008, 3, 72, (Color){ 110, 110, 110, 255 });
    }

    DrawPersonagem(player);

    if (iniciandoTransicao) {
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, fadeAlpha));
    }
}
