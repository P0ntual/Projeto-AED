#include "personagem.h"

#define IDLE_COLS     8
#define IDLE_FRAME_W 48
#define IDLE_FRAME_H 64
#define IDLE_FPS      8.0f

static Texture2D texIdle;
static bool texCarregada = false;
static int  frameAtual   = 0;
static float tempoFrame  = 0.0f;

void InicializarPersonagem(Personagem *p){
    p->posicao.x = 50.0f;
    p->posicao.y = 540.0f;
    p->raio = 20.0f;
    p->velocidade = 6.5f;
    InicializarInventario(&p->inventario);
}

void AtualizarPersonagem(Personagem *p){
    if (IsKeyDown(KEY_W)) p->posicao.y -= p->velocidade;
    if (IsKeyDown(KEY_A)) p->posicao.x -= p->velocidade;
    if (IsKeyDown(KEY_S)) p->posicao.y += p->velocidade;
    if (IsKeyDown(KEY_D)) p->posicao.x += p->velocidade;

    tempoFrame += GetFrameTime();
    if (tempoFrame >= 1.0f / IDLE_FPS) {
        tempoFrame = 0.0f;
        frameAtual = (frameAtual + 1) % IDLE_COLS;
    }
}

void DrawPersonagem(Personagem p){
    if (!texCarregada) {
        texIdle = LoadTexture("assets/images/zelezinho/idle.png");
        texCarregada = true;
    }

    Rectangle src = {
        (float)(frameAtual * IDLE_FRAME_W),
        0.0f,
        (float)IDLE_FRAME_W,
        (float)IDLE_FRAME_H
    };

    float drawW = IDLE_FRAME_W * 3.0f;   /* 144 px */
    float drawH = IDLE_FRAME_H * 3.0f;   /* 192 px */
    Rectangle dst    = { p.posicao.x, p.posicao.y, drawW, drawH };
    Vector2   origin = { drawW / 2.0f, drawH / 2.0f };

    DrawTexturePro(texIdle, src, dst, origin, 0.0f, WHITE);
}
