#include "grafo.h"
#include <stdlib.h>

static const char *nomesSalas[MAX_SALAS] = {
    [SALA_SAGUAO]        = "Saguao",
    [SALA_CORREDOR_1]    = "Corredor 1",
    [SALA_CINEMA_1]      = "Sala 1",
    [SALA_CORREDOR_2]    = "Corredor 2",
    [SALA_CINEMA_2]      = "Sala 2",
    [SALA_BANHEIRO_FEM]  = "Banheiro Feminino",
    [SALA_BANHEIRO_MASC] = "Banheiro Masculino",
    [SALA_ZELADOR]       = "Sala do Zelador",
    [SALA_SAIDA]         = "Saida"
};

static Aresta *novaAresta(IdSala destino, TipoItem chave,
                          Rectangle gatilho, Vector2 spawn) {
    Aresta *a = (Aresta *)malloc(sizeof(Aresta));
    a->destino          = destino;
    a->chaveNecessaria  = chave;
    a->bloqueada        = (chave != ITEM_VAZIO);
    a->gatilho          = gatilho;
    a->posSpawnDestino  = spawn;
    a->proxima          = NULL;
    return a;
}

static void adicionarAresta(NoSala *no, Aresta *a) {
    a->proxima = no->adjacencias;
    no->adjacencias = a;
}

void InicializarGrafo(GrafoMapa *g) {
    for (int i = 0; i < MAX_SALAS; i++) {
        g->nos[i].id          = (IdSala)i;
        g->nos[i].nome        = nomesSalas[i];
        g->nos[i].adjacencias = NULL;
    }

    ConectarSalas(g, SALA_SAGUAO, SALA_CORREDOR_1,
        ITEM_VAZIO,
        (Rectangle){    0.0f,   0.0f, 200.0f, 200.0f }, (Vector2){  300.0f, 280.0f },
        (Rectangle){ 1820.0f, 400.0f, 100.0f, 280.0f }, (Vector2){ 1700.0f, 540.0f });

    ConectarSalas(g, SALA_SAGUAO, SALA_CORREDOR_2,
        ITEM_VAZIO,
        (Rectangle){ 1720.0f,   0.0f, 200.0f, 200.0f }, (Vector2){ 1620.0f, 280.0f },
        (Rectangle){    0.0f, 400.0f, 100.0f, 280.0f }, (Vector2){  220.0f, 540.0f });

    ConectarSalas(g, SALA_SAGUAO, SALA_BANHEIRO_FEM,
        ITEM_CHAVE_BANHEIRO_FEM,
        (Rectangle){    0.0f, 450.0f, 100.0f, 200.0f }, (Vector2){  220.0f, 550.0f },
        (Rectangle){ 1820.0f, 450.0f, 100.0f, 200.0f }, (Vector2){ 1670.0f, 550.0f });

    ConectarSalas(g, SALA_SAGUAO, SALA_BANHEIRO_MASC,
        ITEM_CHAVE_BANHEIRO_MASC,
        (Rectangle){ 1820.0f, 450.0f, 100.0f, 200.0f }, (Vector2){ 1700.0f, 550.0f },
        (Rectangle){    0.0f, 450.0f, 100.0f, 200.0f }, (Vector2){  250.0f, 550.0f });

    ConectarSalas(g, SALA_SAGUAO, SALA_ZELADOR,
        ITEM_CHAVE,
        (Rectangle){ 860.0f,   0.0f, 200.0f, 150.0f }, (Vector2){ 960.0f, 280.0f },
        (Rectangle){ 760.0f, 900.0f, 400.0f, 120.0f }, (Vector2){ 960.0f, 700.0f });

    ConectarSalas(g, SALA_CORREDOR_1, SALA_CINEMA_1,
        ITEM_CHAVE_SALA1,
        (Rectangle){    0.0f, 400.0f, 100.0f, 280.0f }, (Vector2){  220.0f, 540.0f },
        (Rectangle){ 1820.0f, 860.0f, 100.0f, 220.0f }, (Vector2){ 1700.0f, 950.0f });

    ConectarSalas(g, SALA_CORREDOR_2, SALA_CINEMA_2,
        ITEM_CHAVE_SALA2,
        (Rectangle){ 1820.0f, 400.0f, 100.0f, 280.0f }, (Vector2){ 1700.0f, 540.0f },
        (Rectangle){    0.0f, 860.0f, 100.0f, 220.0f }, (Vector2){  220.0f, 950.0f });

    ConectarSalas(g, SALA_SAGUAO, SALA_SAIDA,
        ITEM_VAZIO,
        (Rectangle){ 760.0f, 900.0f, 400.0f, 160.0f }, (Vector2){ 960.0f, 1050.0f },
        (Rectangle){   0.0f,   0.0f,   0.0f,   0.0f }, (Vector2){ 960.0f,  960.0f });
}

void LiberarGrafo(GrafoMapa *g) {
    for (int i = 0; i < MAX_SALAS; i++) {
        Aresta *cur = g->nos[i].adjacencias;
        while (cur) {
            Aresta *prox = cur->proxima;
            free(cur);
            cur = prox;
        }
        g->nos[i].adjacencias = NULL;
    }
}

void ConectarSalas(GrafoMapa *g,
                   IdSala a, IdSala b,
                   TipoItem chave,
                   Rectangle gatilhoEmA, Vector2 spawnEmA,
                   Rectangle gatilhoEmB, Vector2 spawnEmB) {
    adicionarAresta(&g->nos[a], novaAresta(b, chave, gatilhoEmA, spawnEmB));
    adicionarAresta(&g->nos[b], novaAresta(a, chave, gatilhoEmB, spawnEmA));
}

Aresta *ArestaEntre(const GrafoMapa *g, IdSala origem, IdSala destino) {
    Aresta *cur = g->nos[origem].adjacencias;
    while (cur) {
        if (cur->destino == destino) return cur;
        cur = cur->proxima;
    }
    return NULL;
}

void BloquearAcesso(GrafoMapa *g, IdSala a, IdSala b) {
    Aresta *ab = ArestaEntre(g, a, b);
    Aresta *ba = ArestaEntre(g, b, a);
    if (ab) ab->bloqueada = true;
    if (ba) ba->bloqueada = true;
}

void DesbloquearAcesso(GrafoMapa *g, IdSala a, IdSala b) {
    Aresta *ab = ArestaEntre(g, a, b);
    Aresta *ba = ArestaEntre(g, b, a);
    if (ab) ab->bloqueada = false;
    if (ba) ba->bloqueada = false;
}

Aresta *VerificarColisaoPorta(const GrafoMapa *g,
                              IdSala salaAtual,
                              Vector2 posJogador,
                              float raio) {
    Aresta *cur = g->nos[salaAtual].adjacencias;
    while (cur) {
        if (CheckCollisionCircleRec(posJogador, raio, cur->gatilho))
            return cur;
        cur = cur->proxima;
    }
    return NULL;
}

void IrParaSala(IdSala *atual, Vector2 *posJogador, const Aresta *porta) {
    *atual      = porta->destino;
    *posJogador = porta->posSpawnDestino;
}
