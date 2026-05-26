#include "expediente.h"
#include <stdlib.h>

static NoTurno *novoTurno(int hora, const char *rotulo,
                          int aparicoes, float duracao) {
    NoTurno *t = (NoTurno *)malloc(sizeof(NoTurno));
    t->hora                 = hora;
    t->rotulo               = rotulo;
    t->aparicoesPlanejadas  = aparicoes;
    t->duracao              = duracao;
    t->proximo              = NULL;
    return t;
}

void IniciarExpediente(ListaExpediente *exp) {
    NoTurno *t22 = novoTurno(22, "22h", 2, 30.0f);
    NoTurno *t23 = novoTurno(23, "23h", 3, 30.0f);
    NoTurno *t00 = novoTurno( 0, "00h", 4, 30.0f);
    NoTurno *t01 = novoTurno( 1, "01h", 5, 30.0f);
    NoTurno *t02 = novoTurno( 2, "02h", 6, 30.0f);
    NoTurno *t03 = novoTurno( 3, "03h", 7, 30.0f);
    NoTurno *t04 = novoTurno( 4, "04h", 8, 30.0f);
    NoTurno *t05 = novoTurno( 5, "05h", 9, 30.0f);

    t22->proximo = t23;
    t23->proximo = t00;
    t00->proximo = t01;
    t01->proximo = t02;
    t02->proximo = t03;
    t03->proximo = t04;
    t04->proximo = t05;

    exp->cabeca       = t22;
    exp->atual        = t22;
    exp->tempoNoTurno = 0.0f;
}

void LiberarExpediente(ListaExpediente *exp) {
    NoTurno *cur = exp->cabeca;
    while (cur != NULL) {
        NoTurno *prox = cur->proximo;
        free(cur);
        cur = prox;
    }
    exp->cabeca       = NULL;
    exp->atual        = NULL;
    exp->tempoNoTurno = 0.0f;
}

void AvancarTurno(ListaExpediente *exp) {
    if (exp->atual != NULL) {
        exp->atual        = exp->atual->proximo;
        exp->tempoNoTurno = 0.0f;
    }
}

void AtualizarExpediente(ListaExpediente *exp, float deltaTempo) {
    if (exp->atual == NULL) return;
    exp->tempoNoTurno += deltaTempo;
    if (exp->tempoNoTurno >= exp->atual->duracao) {
        AvancarTurno(exp);
    }
}

bool VerificarVitoria(const ListaExpediente *exp) {
    return exp->atual == NULL;
}

const NoTurno *TurnoAtual(const ListaExpediente *exp) {
    return exp->atual;
}
