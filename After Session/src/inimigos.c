#include "inimigos.h"
#include <stdlib.h>
#include <string.h>

void InicializarFilaInimigos(FilaInimigos *fila, float intervalo) {
    fila->inicio = 0;
    fila->fim = 0;
    fila->tamanho = 0;
    fila->tempoProximaAparicao = 0.0f;
    fila->intervaloAparicao = intervalo;
    fila->permiteAparicao = false;

    for (int i = 0; i < FILA_INIMIGOS_MAX; i++) {
        fila->inimigos[i].ativo = false;
        fila->inimigos[i].tempoVida = 0.0f;
    }
}

void EnfileirarInimigo(FilaInimigos *fila, Inimigo inimigo) {
    if (FilaEstaCheia(fila)) return;

    inimigo.ativo = false;
    inimigo.tempoVida = 0.0f;
    fila->inimigos[fila->fim] = inimigo;
    fila->fim = (fila->fim + 1) % FILA_INIMIGOS_MAX;
    fila->tamanho++;
}

Inimigo DesenfileirarInimigo(FilaInimigos *fila) {
    Inimigo inimigo = fila->inimigos[fila->inicio];
    fila->inicio = (fila->inicio + 1) % FILA_INIMIGOS_MAX;
    fila->tamanho--;
    return inimigo;
}

bool FilaEstaVazia(const FilaInimigos *fila) {
    return fila->tamanho == 0;
}

bool FilaEstaCheia(const FilaInimigos *fila) {
    return fila->tamanho >= FILA_INIMIGOS_MAX;
}

int TamanhoFila(const FilaInimigos *fila) {
    return fila->tamanho;
}

void AtualizarFilaInimigos(FilaInimigos *fila) {
    if (!fila->permiteAparicao) return;

    fila->tempoProximaAparicao -= GetFrameTime();

    if (fila->tempoProximaAparicao <= 0.0f && !FilaEstaVazia(fila)) {
        Inimigo inimigo = DesenfileirarInimigo(fila);
        inimigo.ativo = true;

        for (int i = 0; i < FILA_INIMIGOS_MAX; i++) {
            if (!fila->inimigos[i].ativo) {
                fila->inimigos[i] = inimigo;
                break;
            }
        }
        fila->tempoProximaAparicao = fila->intervaloAparicao;
    }
}

Inimigo *ObterProximoInimigo(FilaInimigos *fila) {
    if (!FilaEstaVazia(fila)) {
        return &fila->inimigos[fila->inicio];
    }
    return NULL;
}

void RemoverInimigoAtivo(FilaInimigos *fila, int indice) {
    if (indice >= 0 && indice < FILA_INIMIGOS_MAX) {
        fila->inimigos[indice].ativo = false;
    }
}

void AtivarAparicaoInimigos(FilaInimigos *fila) {
    fila->permiteAparicao = true;
}

void DesativarAparicaoInimigos(FilaInimigos *fila) {
    fila->permiteAparicao = false;
}

bool AparicaoEstaAtiva(const FilaInimigos *fila) {
    return fila->permiteAparicao;
}

void DrawInimigo(Inimigo inimigo) {
    if (!inimigo.ativo) return;

    Color cor;
    switch (inimigo.tipo) {
        case 0: cor = WHITE; break;
        case 1: cor = RED; break;
        case 2: cor = BLUE; break;
        default: cor = PURPLE; break;
    }

    DrawCircleV(inimigo.posicao, inimigo.raio, cor);
    DrawCircleLinesV(inimigo.posicao, inimigo.raio, DARKGRAY);
}

void DrawFilaInimigos(const FilaInimigos *fila) {
    for (int i = 0; i < FILA_INIMIGOS_MAX; i++) {
        if (fila->inimigos[i].ativo) {
            DrawInimigo(fila->inimigos[i]);
        }
    }
}
