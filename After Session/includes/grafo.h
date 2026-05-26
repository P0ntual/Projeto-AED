#ifndef GRAFO_H
#define GRAFO_H

#include "raylib.h"
#include "item.h"
#include <stdbool.h>

typedef enum {
    SALA_SAGUAO = 0,
    SALA_CORREDOR_1,
    SALA_CINEMA_1,
    SALA_CORREDOR_2,
    SALA_CINEMA_2,
    SALA_BANHEIRO_FEM,
    SALA_BANHEIRO_MASC,
    SALA_ZELADOR,
    SALA_SAIDA,
    MAX_SALAS
} IdSala;

typedef struct Aresta {
    IdSala         destino;
    TipoItem       chaveNecessaria;
    bool           bloqueada;
    Rectangle      gatilho;
    Vector2        posSpawnDestino;
    struct Aresta *proxima;
} Aresta;

typedef struct {
    IdSala       id;
    const char  *nome;
    Aresta      *adjacencias;
} NoSala;

typedef struct {
    NoSala nos[MAX_SALAS];
} GrafoMapa;

void InicializarGrafo(GrafoMapa *g);
void LiberarGrafo(GrafoMapa *g);

void ConectarSalas(GrafoMapa *g,
                   IdSala a, IdSala b,
                   TipoItem chave,
                   Rectangle gatilhoEmA, Vector2 spawnEmA,
                   Rectangle gatilhoEmB, Vector2 spawnEmB);

void BloquearAcesso(GrafoMapa *g, IdSala a, IdSala b);
void DesbloquearAcesso(GrafoMapa *g, IdSala a, IdSala b);

Aresta *ArestaEntre(const GrafoMapa *g, IdSala origem, IdSala destino);

Aresta *VerificarColisaoPorta(const GrafoMapa *g,
                              IdSala salaAtual,
                              Vector2 posJogador,
                              float raio);

void IrParaSala(IdSala *atual, Vector2 *posJogador, const Aresta *porta);

#endif
