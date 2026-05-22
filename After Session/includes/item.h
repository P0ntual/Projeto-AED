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
    SlotInventario nos[INVENTARIO_SLOTS]; /* nós alocados estaticamente */
    SlotInventario *cabeca;               /* primeiro nó da lista        */
    SlotInventario *ativo;                /* nó atualmente selecionado   */
    int tamanho;                          /* slots ocupados              */
} Inventario;

/* Operações sobre a lista duplamente encadeada */
void InicializarInventario(Inventario *inv);
bool AdicionarItem(Inventario *inv, TipoItem tipo);
void RemoverItemAtivo(Inventario *inv);
bool BuscarItem(const Inventario *inv, TipoItem tipo);
void TrocarSlotAtivo(Inventario *inv, int indice);
bool InventarioEstaVazio(const Inventario *inv);
bool InventarioEstaCheio(const Inventario *inv);

/* Utilitários e renderização */
const char *NomeItem(TipoItem tipo);
Color CorItem(TipoItem tipo);
void DrawInventario(Inventario inv);

#endif
