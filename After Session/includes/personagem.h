#ifndef PERSONAGEM_H
#define PERSONAGEM_H
#include "raylib.h"
#include <stdbool.h>

typedef struct Personagem {
    Vector2 posicao;
    float velocidade;
    float raio;
    float direcao;
}Personagem;

void InicializarPersonagem(Personagem *p);
void AtualizarPersonagem(Personagem *p);
void DrawPersonagem(Personagem p);

#endif  