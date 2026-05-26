#ifndef RANKING_H
#define RANKING_H

#include <stdbool.h>

#define RANKING_TAMANHO  10
#define RANKING_NOME_MAX 13

typedef struct {
    char  nome[RANKING_NOME_MAX];
    float tempo;
} Pontuacao;

typedef struct {
    Pontuacao entradas[RANKING_TAMANHO];
    int       quantidade;
} Ranking;

void CarregarRanking(Ranking *r);
void SalvarRanking(const Ranking *r);
bool InserirPontuacao(Ranking *r, const char *nome, float tempo);

#endif
