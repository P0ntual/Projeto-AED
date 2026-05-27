#include "inimigos.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

void EnfileirarAparicao(FilaInimigos *fila, Inimigo inimigo) {
    EnfileirarInimigo(fila, inimigo);
}

Inimigo LiberarAparicao(FilaInimigos *fila) {
    return DesenfileirarInimigo(fila);
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

bool AtualizarFilaInimigos(FilaInimigos *fila, Vector2 jogadorPos,
                           int x0, int y0, int x1, int y1) {
    if (!fila->permiteAparicao) return false;

    fila->tempoProximaAparicao -= GetFrameTime();

    if (fila->tempoProximaAparicao <= 0.0f && !FilaEstaVazia(fila)) {
        Inimigo inimigo = DesenfileirarInimigo(fila);
        inimigo.ativo = true;

        const float distMin = 300.0f;
        const float distMinSqr = distMin * distMin;
        for (int tentativa = 0; tentativa < 30; tentativa++) {
            inimigo.posicao.x = (float)GetRandomValue(x0, x1);
            inimigo.posicao.y = (float)GetRandomValue(y0, y1);
            float dx = inimigo.posicao.x - jogadorPos.x;
            float dy = inimigo.posicao.y - jogadorPos.y;
            if (dx*dx + dy*dy >= distMinSqr) break;
        }

        for (int i = 0; i < FILA_INIMIGOS_MAX; i++) {
            if (!fila->inimigos[i].ativo) {
                fila->inimigos[i] = inimigo;
                break;
            }
        }
        fila->tempoProximaAparicao = fila->intervaloAparicao;
        return true;
    }
    return false;
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

void AtualizarMovimentoInimigos(FilaInimigos *fila, Vector2 alvo) {
    for (int i = 0; i < FILA_INIMIGOS_MAX; i++) {
        Inimigo *inimigo = &fila->inimigos[i];
        if (!inimigo->ativo) continue;

        float dx = alvo.x - inimigo->posicao.x;
        float dy = alvo.y - inimigo->posicao.y;
        float distSqr = dx*dx + dy*dy;

        if (inimigo->tipo == INIMIGO_WITCH &&
            distSqr > WITCH_RAIO_DETECCAO * WITCH_RAIO_DETECCAO) {
            continue;
        }

        float dist = sqrtf(distSqr);
        if (dist > 0.001f) {
            inimigo->posicao.x += (dx / dist) * inimigo->velocidade;
            inimigo->posicao.y += (dy / dist) * inimigo->velocidade;
        }
    }
}

bool ColisaoComInimigos(const FilaInimigos *fila, Vector2 pos, float raio) {
    for (int i = 0; i < FILA_INIMIGOS_MAX; i++) {
        const Inimigo *inimigo = &fila->inimigos[i];
        if (!inimigo->ativo) continue;

        float dx = pos.x - inimigo->posicao.x;
        float dy = pos.y - inimigo->posicao.y;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist <= raio + inimigo->raio) return true;
    }
    return false;
}

void DrawInimigo(Inimigo inimigo) {
    if (!inimigo.ativo) return;

    Color cor = (inimigo.tipo == INIMIGO_WITCH) ? PURPLE : WHITE;

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
