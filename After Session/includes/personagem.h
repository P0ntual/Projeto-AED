#ifndef PERSONAGEM_H
#define PERSONAGEM_H
#include "raylib.h"
#include "item.h"
#include <stdbool.h>

typedef struct Personagem {
    Vector2 posicao;
    float velocidade;
    float raio;
    float direcao;
    Inventario inventario;
} Personagem;

void InicializarPersonagem(Personagem *p);
void AtualizarPersonagem(Personagem *p);
void DrawPersonagem(Personagem p);

#endif  