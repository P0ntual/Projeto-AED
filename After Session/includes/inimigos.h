#ifndef INIMIGOS_H
#define INIMIGOS_H

#include "raylib.h"
#include <stdbool.h>

typedef struct {
    Vector2 posicao;
    float velocidade;
    float raio;
    int tipo;
    bool ativo;
    float tempoVida;
} Inimigo;

#define FILA_INIMIGOS_MAX 20

typedef struct {
    Inimigo inimigos[FILA_INIMIGOS_MAX];
    int inicio;
    int fim;
    int tamanho;
    float tempoProximaAparicao;
    float intervaloAparicao;
    bool permiteAparicao;
} FilaInimigos;

void InicializarFilaInimigos(FilaInimigos *fila, float intervalo);
void EnfileirarInimigo(FilaInimigos *fila, Inimigo inimigo);
Inimigo DesenfileirarInimigo(FilaInimigos *fila);
bool FilaEstaVazia(const FilaInimigos *fila);
bool FilaEstaCheia(const FilaInimigos *fila);
int TamanhoFila(const FilaInimigos *fila);

void AtualizarFilaInimigos(FilaInimigos *fila);
Inimigo *ObterProximoInimigo(FilaInimigos *fila);
void RemoverInimigoAtivo(FilaInimigos *fila, int indice);

void AtivarAparicaoInimigos(FilaInimigos *fila);
void DesativarAparicaoInimigos(FilaInimigos *fila);
bool AparicaoEstaAtiva(const FilaInimigos *fila);

void DrawInimigo(Inimigo inimigo);
void DrawFilaInimigos(const FilaInimigos *fila);

#endif
