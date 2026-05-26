#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "telas.h"
#include "personagem.h"
#include "inimigos.h"


TelaAtual UpdateGameplay(Personagem *player);


void DrawGameplay(Personagem player);

float GameplayTempoDecorrido(void);

#endif