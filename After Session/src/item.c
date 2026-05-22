#include "item.h"

/*
 * Liga os INVENTARIO_SLOTS nós em sequência (lista duplamente encadeada),
 * inicializa todos os campos e posiciona cabeca e ativo no primeiro nó.
 */
void InicializarInventario(Inventario *inv) {
    for (int i = 0; i < INVENTARIO_SLOTS; i++) {
        inv->nos[i].item     = ITEM_VAZIO;
        inv->nos[i].indice   = i;
        inv->nos[i].anterior = (i > 0)                  ? &inv->nos[i - 1] : NULL;
        inv->nos[i].proximo  = (i < INVENTARIO_SLOTS - 1) ? &inv->nos[i + 1] : NULL;
    }
    inv->cabeca  = &inv->nos[0];
    inv->ativo   = &inv->nos[0];
    inv->tamanho = 0;
}

/*
 * Percorre a lista de cabeca→proximo e insere o item no primeiro nó vazio.
 * Retorna false se todos os nós estiverem ocupados (lista cheia).
 */
bool AdicionarItem(Inventario *inv, TipoItem tipo) {
    SlotInventario *cur = inv->cabeca;
    while (cur != NULL) {
        if (cur->item == ITEM_VAZIO) {
            cur->item = tipo;
            inv->tamanho++;
            return true;
        }
        cur = cur->proximo;
    }
    return false;
}

/*
 * Remove o item do nó apontado por 'ativo', deixando o slot vazio.
 */
void RemoverItemAtivo(Inventario *inv) {
    if (inv->ativo->item != ITEM_VAZIO) {
        inv->ativo->item = ITEM_VAZIO;
        inv->tamanho--;
    }
}

/*
 * Percorre a lista linearmente procurando um nó com o tipo informado.
 * Retorna true se encontrado; false caso contrário.
 */
bool BuscarItem(const Inventario *inv, TipoItem tipo) {
    const SlotInventario *cur = inv->cabeca;
    while (cur != NULL) {
        if (cur->item == tipo) return true;
        cur = cur->proximo;
    }
    return false;
}

/*
 * Percorre a lista do cabeça até o nó de índice desejado e atualiza 'ativo'.
 * A navegação usa os ponteiros proximo — é aqui que a lista encadeada
 * substitui o acesso direto por índice.
 */
void TrocarSlotAtivo(Inventario *inv, int indice) {
    if (indice < 0 || indice >= INVENTARIO_SLOTS) return;
    SlotInventario *cur = inv->cabeca;
    while (cur != NULL) {
        if (cur->indice == indice) {
            inv->ativo = cur;
            return;
        }
        cur = cur->proximo;
    }
}

/* Verifica em O(1) se nenhum slot está ocupado. */
bool InventarioEstaVazio(const Inventario *inv) {
    return inv->tamanho == 0;
}

/* Verifica em O(1) se todos os slots estão ocupados. */
bool InventarioEstaCheio(const Inventario *inv) {
    return inv->tamanho == INVENTARIO_SLOTS;
}

const char *NomeItem(TipoItem tipo) {
    switch (tipo) {
        case ITEM_CHAVE:     return "Chave";
        case ITEM_VASSOURA:  return "Vassoura";
        case ITEM_LANTERNA:  return "Lanterna";
        case ITEM_SACO_LIXO: return "Saco";
        case ITEM_PILHA:     return "Pilha";
        default:             return "";
    }
}

Color CorItem(TipoItem tipo) {
    switch (tipo) {
        case ITEM_CHAVE:     return GOLD;
        case ITEM_VASSOURA:  return BROWN;
        case ITEM_LANTERNA:  return YELLOW;
        case ITEM_SACO_LIXO: return DARKGREEN;
        case ITEM_PILHA:     return LIME;
        default:             return BLANK;
    }
}

/*
 * Percorre a lista de cabeca→proximo para renderizar os slots em ordem.
 * 'cur == inv.ativo' funciona corretamente porque ambos os ponteiros
 * referenciam os nós do struct original em main.c (não a cópia local).
 */
void DrawInventario(Inventario inv) {
    const int SLOT_SIZE = 80;
    const int SLOT_GAP  = 10;
    const int TOTAL_W   = INVENTARIO_SLOTS * SLOT_SIZE + (INVENTARIO_SLOTS - 1) * SLOT_GAP;
    const int START_X   = (1920 - TOTAL_W) / 2;
    const int START_Y   = 1080 - SLOT_SIZE - 20;

    SlotInventario *cur = inv.cabeca;
    while (cur != NULL) {
        int x     = START_X + cur->indice * (SLOT_SIZE + SLOT_GAP);
        bool ativo = (cur == inv.ativo);

        DrawRectangle(x, START_Y, SLOT_SIZE, SLOT_SIZE, (Color){ 30, 30, 30, 200 });

        float espessura = ativo ? 3.0f : 1.0f;
        Color bordaCor  = ativo ? WHITE : (Color){ 80, 80, 80, 255 };
        DrawRectangleLinesEx(
            (Rectangle){ (float)x, (float)START_Y, (float)SLOT_SIZE, (float)SLOT_SIZE },
            espessura, bordaCor);

        if (cur->item != ITEM_VAZIO) {
            DrawRectangle(x + 12, START_Y + 10, SLOT_SIZE - 24, SLOT_SIZE - 38,
                          CorItem(cur->item));
            DrawText(NomeItem(cur->item), x + 5, START_Y + SLOT_SIZE - 22, 14, WHITE);
        }

        char numStr[2] = { '1' + cur->indice, '\0' };
        DrawText(numStr, x + 5, START_Y + 5, 16, ativo ? WHITE : GRAY);

        cur = cur->proximo;
    }
}
