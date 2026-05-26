#include "item.h"
#include <stdlib.h>

static bool itemMenor(TipoItem a, TipoItem b) {
    if (a == ITEM_VAZIO) return false;
    if (b == ITEM_VAZIO) return true;
    return a < b;
}

static void recomputarIndices(Inventario *inv) {
    SlotInventario *cur = inv->cabeca;
    int idx = 0;
    while (cur != NULL) {
        cur->indice = idx++;
        cur = cur->proximo;
    }
}

void InicializarInventario(Inventario *inv) {
    for (int i = 0; i < INVENTARIO_SLOTS; i++) {
        inv->nos[i].item     = ITEM_VAZIO;
        inv->nos[i].indice   = i;
        inv->nos[i].anterior = (i > 0)                    ? &inv->nos[i - 1] : NULL;
        inv->nos[i].proximo  = (i < INVENTARIO_SLOTS - 1) ? &inv->nos[i + 1] : NULL;
    }
    inv->cabeca  = &inv->nos[0];
    inv->ativo   = &inv->nos[0];
    inv->tamanho = 0;
}

void OrdenarInventario(Inventario *inv) {
    if (inv->cabeca == NULL) return;

    SlotInventario *cur = inv->cabeca->proximo;
    while (cur != NULL) {
        SlotInventario *next = cur->proximo;
        SlotInventario *p    = cur->anterior;

        while (p != NULL && itemMenor(cur->item, p->item)) {
            p = p->anterior;
        }

        if (p != cur->anterior) {
            cur->anterior->proximo = cur->proximo;
            if (cur->proximo) cur->proximo->anterior = cur->anterior;

            if (p == NULL) {
                cur->proximo  = inv->cabeca;
                cur->anterior = NULL;
                inv->cabeca->anterior = cur;
                inv->cabeca = cur;
            } else {
                cur->proximo  = p->proximo;
                cur->anterior = p;
                if (p->proximo) p->proximo->anterior = cur;
                p->proximo = cur;
            }
        }

        cur = next;
    }

    recomputarIndices(inv);
}

bool AdicionarItem(Inventario *inv, TipoItem tipo) {
    for (int i = 0; i < INVENTARIO_SLOTS; i++) {
        if (inv->nos[i].item == ITEM_VAZIO) {
            inv->nos[i].item = tipo;
            inv->tamanho++;
            OrdenarInventario(inv);
            return true;
        }
    }
    return false;
}

void RemoverItemAtivo(Inventario *inv) {
    if (inv->ativo->item != ITEM_VAZIO) {
        inv->ativo->item = ITEM_VAZIO;
        inv->tamanho--;
        OrdenarInventario(inv);
    }
}

bool BuscarItem(const Inventario *inv, TipoItem tipo) {
    const SlotInventario *cur = inv->cabeca;
    while (cur != NULL) {
        if (cur->item == tipo) return true;
        cur = cur->proximo;
    }
    return false;
}

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

bool InventarioEstaVazio(const Inventario *inv) { return inv->tamanho == 0; }
bool InventarioEstaCheio(const Inventario *inv) { return inv->tamanho == INVENTARIO_SLOTS; }

const char *NomeItem(TipoItem tipo) {
    switch (tipo) {
        case ITEM_CHAVE:               return "Chave";
        case ITEM_CHAVE_SALA1:         return "Ch.Sala1";
        case ITEM_CHAVE_SALA2:         return "Ch.Sala2";
        case ITEM_CHAVE_BANHEIRO_FEM:  return "Ch.WCF";
        case ITEM_CHAVE_BANHEIRO_MASC: return "Ch.WCM";
        case ITEM_VASSOURA:            return "Vassoura";
        case ITEM_LANTERNA:            return "Lanterna";
        case ITEM_SACO_LIXO:           return "Saco";
        case ITEM_PILHA:               return "Pilha";
        default:                       return "";
    }
}

Color CorItem(TipoItem tipo) {
    switch (tipo) {
        case ITEM_CHAVE:               return GOLD;
        case ITEM_CHAVE_SALA1:         return GOLD;
        case ITEM_CHAVE_SALA2:         return GOLD;
        case ITEM_CHAVE_BANHEIRO_FEM:  return GOLD;
        case ITEM_CHAVE_BANHEIRO_MASC: return GOLD;
        case ITEM_VASSOURA:            return BROWN;
        case ITEM_LANTERNA:            return YELLOW;
        case ITEM_SACO_LIXO:           return DARKGREEN;
        case ITEM_PILHA:               return LIME;
        default:                       return BLANK;
    }
}

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

/* --- Lista Simplesmente Encadeada (Itens no Chão) --- */

void InicializarListaChao(ListaItensChao *lista) {
    lista->cabeca  = NULL;
    lista->tamanho = 0;
}

void LargarItemChao(ListaItensChao *lista, TipoItem tipo, Vector2 posicao, int mapaId, int dados) {
    ItemChao *novo = (ItemChao *)malloc(sizeof(ItemChao));
    if (novo == NULL) return;
    novo->tipo    = tipo;
    novo->posicao = posicao;
    novo->mapaId  = mapaId;
    novo->dados   = dados;
    novo->proximo = lista->cabeca;
    lista->cabeca = novo;
    lista->tamanho++;
}

ItemChao *ItemChaoProximo(const ListaItensChao *lista, Vector2 posicao, int mapaId, float raio) {
    ItemChao *cur = lista->cabeca;
    while (cur != NULL) {
        if (cur->mapaId == mapaId) {
            float dx = posicao.x - cur->posicao.x;
            float dy = posicao.y - cur->posicao.y;
            if ((dx * dx + dy * dy) <= (raio * raio)) return cur;
        }
        cur = cur->proximo;
    }
    return NULL;
}

TipoItem ColetarItemChao(ListaItensChao *lista, Vector2 posicao, int mapaId, float raio) {
    ItemChao *cur  = lista->cabeca;
    ItemChao *prev = NULL;
    while (cur != NULL) {
        if (cur->mapaId == mapaId) {
            float dx = posicao.x - cur->posicao.x;
            float dy = posicao.y - cur->posicao.y;
            if ((dx * dx + dy * dy) <= (raio * raio)) {
                if (prev == NULL) lista->cabeca  = cur->proximo;
                else              prev->proximo  = cur->proximo;
                TipoItem tipo = cur->tipo;
                free(cur);
                lista->tamanho--;
                return tipo;
            }
        }
        prev = cur;
        cur  = cur->proximo;
    }
    return ITEM_VAZIO;
}

/* --- Lista Circular Encadeada (Sujeiras/Lixos) --- */

void InicializarListaElementos(ListaElementos *lista) {
    lista->cabeca  = NULL;
    lista->tamanho = 0;
}

void InserirElemento(ListaElementos *lista, TipoElemento tipo, Vector2 posicao, int mapaId) {
    NodoElemento *novo = (NodoElemento *)malloc(sizeof(NodoElemento));
    if (novo == NULL) return;
    novo->tipo    = tipo;
    novo->posicao = posicao;
    novo->mapaId  = mapaId;
    if (lista->cabeca == NULL) {
        novo->proximo = novo;
        lista->cabeca = novo;
    } else {
        NodoElemento *tail = lista->cabeca;
        while (tail->proximo != lista->cabeca) tail = tail->proximo;
        tail->proximo = novo;
        novo->proximo = lista->cabeca;
    }
    lista->tamanho++;
}

NodoElemento *ElementoProximo(const ListaElementos *lista, TipoElemento tipo, Vector2 posicao, int mapaId, float raio) {
    if (lista->cabeca == NULL) return NULL;
    NodoElemento *cur = lista->cabeca;
    do {
        if (cur->mapaId == mapaId && cur->tipo == tipo) {
            float dx = posicao.x - cur->posicao.x;
            float dy = posicao.y - cur->posicao.y;
            if ((dx * dx + dy * dy) <= (raio * raio)) return cur;
        }
        cur = cur->proximo;
    } while (cur != lista->cabeca);
    return NULL;
}

bool RemoverElementoProximo(ListaElementos *lista, TipoElemento tipo, Vector2 posicao, int mapaId, float raio) {
    if (lista->cabeca == NULL) return false;
    NodoElemento *tail = lista->cabeca;
    while (tail->proximo != lista->cabeca) tail = tail->proximo;
    NodoElemento *prev = tail;
    NodoElemento *cur  = lista->cabeca;
    do {
        if (cur->mapaId == mapaId && cur->tipo == tipo) {
            float dx = posicao.x - cur->posicao.x;
            float dy = posicao.y - cur->posicao.y;
            if ((dx * dx + dy * dy) <= (raio * raio)) {
                prev->proximo = cur->proximo;
                if (cur == lista->cabeca)
                    lista->cabeca = (lista->tamanho == 1) ? NULL : cur->proximo;
                free(cur);
                lista->tamanho--;
                return true;
            }
        }
        prev = cur;
        cur  = cur->proximo;
    } while (cur != lista->cabeca);
    return false;
}