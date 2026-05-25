#ifndef INTRO_H
#define INTRO_H

#include "telas.h"
#include "personagem.h"

void InitIntro(void);
void UnloadIntro(void);
TelaAtual UpdateIntro(Personagem *player);
void DrawIntro(Personagem player);

#endif