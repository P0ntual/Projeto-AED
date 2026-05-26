#ifndef EXPEDIENTE_H
#define EXPEDIENTE_H

#include <stdbool.h>

typedef struct NoTurno {
    int              hora;
    const char      *rotulo;
    int              aparicoesPlanejadas;
    float            duracao;
    struct NoTurno  *proximo;
} NoTurno;

typedef struct {
    NoTurno *cabeca;
    NoTurno *atual;
    float    tempoNoTurno;
} ListaExpediente;

void IniciarExpediente(ListaExpediente *exp);
void LiberarExpediente(ListaExpediente *exp);

void AvancarTurno(ListaExpediente *exp);
void AtualizarExpediente(ListaExpediente *exp, float deltaTempo);

bool VerificarVitoria(const ListaExpediente *exp);
const NoTurno *TurnoAtual(const ListaExpediente *exp);

#endif
