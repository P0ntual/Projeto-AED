#include "personagem.h"

void InicializarPersonagem(Personagem *p){
    p->posicao.x = 50.0f;
    p->posicao.y = 540.0f;
    p->raio = 20.0f;
    p->velocidade = 6.5f;
}

void AtualizarPersonagem(Personagem *p){
    if (IsKeyDown(KEY_W)) p->posicao.y -= p->velocidade;
    if (IsKeyDown(KEY_A)) p->posicao.x -= p->velocidade;   
    if (IsKeyDown(KEY_S)) p->posicao.y += p->velocidade;
    if (IsKeyDown(KEY_D)) p->posicao.x += p->velocidade;
}

void DrawPersonagem(Personagem p){
    DrawCircle((int)p.posicao.x, (int)p.posicao.y, p.raio, BLUE);
}
