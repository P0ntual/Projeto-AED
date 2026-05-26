#include "ranking.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define RANKING_ARQUIVO "ranking.txt"

void CarregarRanking(Ranking *r) {
    r->quantidade = 0;
    FILE *f = fopen(RANKING_ARQUIVO, "r");
    if (f == NULL) return;

    char linha[64];
    while (fgets(linha, sizeof(linha), f) != NULL && r->quantidade < RANKING_TAMANHO) {
        char *sep = strchr(linha, '|');
        if (sep == NULL) continue;

        size_t nameLen = (size_t)(sep - linha);
        if (nameLen >= RANKING_NOME_MAX) nameLen = RANKING_NOME_MAX - 1;

        Pontuacao *p = &r->entradas[r->quantidade];
        memcpy(p->nome, linha, nameLen);
        p->nome[nameLen] = '\0';
        p->tempo = (float)atof(sep + 1);

        r->quantidade++;
    }
    fclose(f);
}

void SalvarRanking(const Ranking *r) {
    FILE *f = fopen(RANKING_ARQUIVO, "w");
    if (f == NULL) return;
    for (int i = 0; i < r->quantidade; i++) {
        fprintf(f, "%s|%.2f\n", r->entradas[i].nome, r->entradas[i].tempo);
    }
    fclose(f);
}

bool InserirPontuacao(Ranking *r, const char *nome, float tempo) {
    int pos = r->quantidade;
    for (int i = 0; i < r->quantidade; i++) {
        if (tempo < r->entradas[i].tempo) {
            pos = i;
            break;
        }
    }

    if (pos >= RANKING_TAMANHO) return false;

    int end = (r->quantidade < RANKING_TAMANHO) ? r->quantidade : RANKING_TAMANHO - 1;
    for (int i = end; i > pos; i--) {
        r->entradas[i] = r->entradas[i - 1];
    }

    strncpy(r->entradas[pos].nome, nome, RANKING_NOME_MAX - 1);
    r->entradas[pos].nome[RANKING_NOME_MAX - 1] = '\0';
    r->entradas[pos].tempo = tempo;

    if (r->quantidade < RANKING_TAMANHO) r->quantidade++;
    return true;
}
