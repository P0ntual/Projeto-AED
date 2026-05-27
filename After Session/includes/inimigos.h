#ifndef INIMIGOS_H
#define INIMIGOS_H

#include "raylib.h"
#include <stdbool.h>

typedef enum {
    INIMIGO_STALKER = 0,
    INIMIGO_WITCH
} TipoInimigo;

#define WITCH_RAIO_DETECCAO 250.0f

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
void EnfileirarAparicao(FilaInimigos *fila, Inimigo inimigo);
Inimigo LiberarAparicao(FilaInimigos *fila);
bool FilaEstaVazia(const FilaInimigos *fila);
bool FilaEstaCheia(const FilaInimigos *fila);
int TamanhoFila(const FilaInimigos *fila);

bool AtualizarFilaInimigos(FilaInimigos *fila, Vector2 jogadorPos,
                           int x0, int y0, int x1, int y1);
Inimigo *ObterProximoInimigo(FilaInimigos *fila);
void RemoverInimigoAtivo(FilaInimigos *fila, int indice);

void AtualizarMovimentoInimigos(FilaInimigos *fila, Vector2 alvo);
bool ColisaoComInimigos(const FilaInimigos *fila, Vector2 pos, float raio);

void AtivarAparicaoInimigos(FilaInimigos *fila);
void DesativarAparicaoInimigos(FilaInimigos *fila);
bool AparicaoEstaAtiva(const FilaInimigos *fila);

void InitInimigos(void);
void UnloadInimigos(void);
void DrawInimigo(Inimigo inimigo);
void DrawFilaInimigos(const FilaInimigos *fila);

#endif
