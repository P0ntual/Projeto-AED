#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "telas.h"
#include "personagem.h"

// O Update vai cuidar da lógica (colisões com a porta, etc)
TelaAtual UpdateGameplay(Personagem *player);

// O Draw vai desenhar as portas, o cenário e o personagem
void DrawGameplay(Personagem player);

#endif  