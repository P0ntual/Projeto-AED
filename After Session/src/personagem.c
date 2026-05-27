#include "personagem.h"

#define FRAME_W              48
#define FRAME_H              64
#define FRAMES_POR_DIRECAO    8
#define ANIM_FPS              8.0f

typedef enum {
    DIR_DOWN  = 0,
    DIR_LEFT  = 1,
    DIR_RIGHT = 2,
    DIR_UP    = 3
} Direcao;

static Texture2D texSheet;
static bool texCarregada = false;

static int     frameAtual = 0;
static float   tempoFrame = 0.0f;
static Direcao dirAtual   = DIR_DOWN;

void InicializarPersonagem(Personagem *p){
    p->posicao.x = 50.0f;
    p->posicao.y = 540.0f;
    p->raio = 20.0f;
    p->velocidade = 6.5f;
    InicializarInventario(&p->inventario);
}

void AtualizarPersonagem(Personagem *p){
    bool W = IsKeyDown(KEY_W);
    bool A = IsKeyDown(KEY_A);
    bool S = IsKeyDown(KEY_S);
    bool D = IsKeyDown(KEY_D);
    bool movendo = W || A || S || D;

    if (W) p->posicao.y -= p->velocidade;
    if (A) p->posicao.x -= p->velocidade;
    if (S) p->posicao.y += p->velocidade;
    if (D) p->posicao.x += p->velocidade;

    if      (W) dirAtual = DIR_UP;
    else if (S) dirAtual = DIR_DOWN;
    else if (A) dirAtual = DIR_LEFT;
    else if (D) dirAtual = DIR_RIGHT;

    if (movendo) {
        tempoFrame += GetFrameTime();
        if (tempoFrame >= 1.0f / ANIM_FPS) {
            tempoFrame = 0.0f;
            frameAtual = (frameAtual + 1) % FRAMES_POR_DIRECAO;
        }
    } else {
        frameAtual = 0;
        tempoFrame = 0.0f;
    }
}

void DrawPersonagem(Personagem p){
    if (!texCarregada) {
        texSheet = LoadTexture("assets/images/zelezinho/idle.png");
        texCarregada = true;
    }

    int   rowSprite = (dirAtual == DIR_RIGHT) ? DIR_LEFT : dirAtual;
    float largura   = (dirAtual == DIR_RIGHT) ? -(float)FRAME_W : (float)FRAME_W;

    Rectangle src = {
        (float)(frameAtual * FRAME_W),
        (float)(rowSprite  * FRAME_H),
        largura,
        (float)FRAME_H
    };

    float drawW = FRAME_W * 3.0f;
    float drawH = FRAME_H * 3.0f;
    Rectangle dst = {
        p.posicao.x - drawW / 2.0f,
        p.posicao.y - drawH / 2.0f,
        drawW, drawH
    };

    DrawTexturePro(texSheet, src, dst, (Vector2){0, 0}, 0.0f, WHITE);
}
