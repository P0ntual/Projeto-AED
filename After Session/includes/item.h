#ifndef ITEM_H
#define ITEM_H

#include "raylib.h"
#include <stdbool.h>
#include <stddef.h>

/*
 * ESTRUTURA DE DADOS: Lista Duplamente Encadeada
 *
 * Cada slot do inventário é um nó (SlotInventario) ligado ao anterior e ao próximo.
 * O ponteiro 'cabeca' marca o início da lista.
 * O ponteiro 'ativo' percorre a lista para identificar o slot selecionado pelo jogador.
 *
 * A estrutura participa diretamente da lógica:
 *   - AdicionarItem   : percorre cabeca→proximo até achar o primeiro nó vazio
 *   - RemoverItemAtivo: usa o ponteiro 'ativo' para acessar o nó diretamente
 *   - BuscarItem      : percorre a lista linearmente por tipo
 *   - TrocarSlotAtivo : percorre do cabeça até o nó de índice desejado
 *   - DrawInventario  : percorre do cabeça ao fim para renderizar os slots em ordem
 */

typedef enum {
    ITEM_VAZIO = 0,
    ITEM_CHAVE,
    ITEM_CHAVE_SALA1,
    ITEM_CHAVE_SALA2,
    ITEM_CHAVE_BANHEIRO_FEM,
    ITEM_CHAVE_BANHEIRO_MASC,
    ITEM_VASSOURA,
    ITEM_LANTERNA,
    ITEM_SACO_LIXO,
    ITEM_PILHA
} TipoItem;

#define INVENTARIO_SLOTS 3

typedef struct SlotInventario {
    TipoItem item;
    int indice;
    struct SlotInventario *anterior;
    struct SlotInventario *proximo;
} SlotInventario;

typedef struct {
    SlotInventario nos[INVENTARIO_SLOTS];
    SlotInventario *cabeca;
    SlotInventario *ativo;
    int tamanho;
} Inventario;

void InicializarInventario(Inventario *inv);
bool AdicionarItem(Inventario *inv, TipoItem tipo);
void RemoverItemAtivo(Inventario *inv);
bool BuscarItem(const Inventario *inv, TipoItem tipo);
void TrocarSlotAtivo(Inventario *inv, int indice);
bool InventarioEstaVazio(const Inventario *inv);
bool InventarioEstaCheio(const Inventario *inv);

const char *NomeItem(TipoItem tipo);
Color CorItem(TipoItem tipo);
void DrawInventario(Inventario inv);

/* -----------------------------------------------------------------
 * ITENS NO CHÃO — Lista Simplesmente Encadeada
 * ----------------------------------------------------------------- */

typedef struct ItemChao {
    TipoItem tipo;
    Vector2  posicao;
    int      mapaId;
    int      dados;
    struct ItemChao *proximo;
} ItemChao;

typedef struct {
    ItemChao *cabeca;
    int       tamanho;
} ListaItensChao;

void      InicializarListaChao(ListaItensChao *lista);
void      LargarItemChao(ListaItensChao *lista, TipoItem tipo, Vector2 posicao, int mapaId, int dados);
ItemChao *ItemChaoProximo(const ListaItensChao *lista, Vector2 posicao, int mapaId, float raio);
TipoItem  ColetarItemChao(ListaItensChao *lista, Vector2 posicao, int mapaId, float raio);

/* -----------------------------------------------------------------
 * ELEMENTOS NO CHÃO (SUJEIRAS e LIXOS) — Lista Circular Encadeada
 * ----------------------------------------------------------------- */

typedef enum {
    ELEMENTO_SUJEIRA,
    ELEMENTO_LIXO
} TipoElemento;

typedef struct NodoElemento {
    TipoElemento tipo;
    Vector2 posicao;
    int mapaId;
    struct NodoElemento *proximo;
} NodoElemento;

typedef struct {
    NodoElemento *cabeca;
    int tamanho;
} ListaElementos;

void          InicializarListaElementos(ListaElementos *lista);
void          InserirElemento(ListaElementos *lista, TipoElemento tipo, Vector2 posicao, int mapaId);
NodoElemento *ElementoProximo(const ListaElementos *lista, TipoElemento tipo, Vector2 posicao, int mapaId, float raio);
bool          RemoverElementoProximo(ListaElementos *lista, TipoElemento tipo, Vector2 posicao, int mapaId, float raio);

#endif