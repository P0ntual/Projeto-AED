#include "gameplay.h"
#include "item.h"
#include "raylib.h"
#include <stdbool.h>


typedef enum {
    MAPA_SAGUAO,
    MAPA_CORREDOR_1,
    MAPA_SALA_1,
    MAPA_CORREDOR_2,
    MAPA_SALA_2,
    MAPA_BANHEIRO_FEM,
    MAPA_BANHEIRO_MASC,
    MAPA_SALA_ZELADOR
} MapaAtual;

static MapaAtual mapaAtual = MAPA_SAGUAO; 

static Rectangle portaCorredor1 = { 0.0f, 0.0f, 200.0f, 200.0f };
static Rectangle portaCorredor2 = { 1720.0f, 0.0f, 200.0f, 200.0f };
static Rectangle portaSaida     = { 760.0f, 900.0f, 400.0f, 160.0f };

// Novas portas no saguão
static Rectangle portaBanheiroFem  = {    0.0f, 450.0f, 100.0f, 200.0f };
static Rectangle portaBanheiroMasc = { 1820.0f, 450.0f, 100.0f, 200.0f };
static Rectangle portaZelador      = {  860.0f,   0.0f, 200.0f, 150.0f };

// Portas Padrão para os Corredores e Salas
static Rectangle portaEsquerda = { 0.0f, 400.0f, 100.0f, 280.0f };
static Rectangle portaDireita  = { 1820.0f, 400.0f, 100.0f, 280.0f };

// Blocos sólidos das salas (poltronas e tela do cinema)
static Rectangle telaBloco        = {  710.0f,  20.0f, 500.0f, 200.0f };
static Rectangle poltronasEsq     = {  100.0f, 300.0f, 360.0f, 480.0f };
static Rectangle poltronasDirr    = { 1460.0f, 300.0f, 360.0f, 480.0f };
static Rectangle poltronasCentEsq = {  560.0f, 300.0f, 300.0f, 480.0f };
static Rectangle poltronasCentDir = { 1060.0f, 300.0f, 300.0f, 480.0f };

// Portas de saída das salas
static Rectangle portaSaidaSala1 = {    0.0f, 860.0f, 100.0f, 220.0f }; // canto inferior esquerdo
static Rectangle portaSaidaSala2 = { 1820.0f, 860.0f, 100.0f, 220.0f }; // canto inferior direito

// Porta de saída dos banheiros e zelador (fundo central → saguão)
static Rectangle portaSaidaQuarto = { 760.0f, 900.0f, 400.0f, 120.0f };

// Blocos sólidos do Banheiro Feminino
static Rectangle cabinesBanheiroFem = {   40.0f,  80.0f, 160.0f, 700.0f };
static Rectangle piasBanheiroFem    = { 1700.0f,  80.0f, 180.0f, 200.0f };

// Blocos sólidos do Banheiro Masculino
static Rectangle cabinesBanheiroMasc = { 1720.0f,  80.0f, 160.0f, 700.0f };
static Rectangle piasBanheiroMasc    = {   40.0f,  80.0f, 180.0f, 200.0f };

// Blocos sólidos da Sala do Zelador
static Rectangle armarioZelador = {  40.0f,  40.0f, 350.0f, 250.0f };
static Rectangle caixasZelador  = { 600.0f, 150.0f, 700.0f, 250.0f };

#define MAX_ITENS_MUNDO 6

typedef struct {
    TipoItem tipo;
    Vector2 posicao;
    float raioColeta;
    bool coletado;
    MapaAtual mapa;
} ItemMundo;

static ItemMundo itensMundo[MAX_ITENS_MUNDO] = {
    { ITEM_CHAVE,     { 960.0f, 500.0f }, 50.0f, false, MAPA_SAGUAO       },
    { ITEM_VASSOURA,  { 430.0f, 100.0f }, 50.0f, false, MAPA_SALA_ZELADOR  },
    { ITEM_LANTERNA,  { 530.0f, 100.0f }, 50.0f, false, MAPA_SALA_ZELADOR  },
    { ITEM_SACO_LIXO, { 430.0f, 170.0f }, 50.0f, false, MAPA_SALA_ZELADOR  },
    { ITEM_SACO_LIXO, { 630.0f, 170.0f }, 50.0f, false, MAPA_SALA_ZELADOR  },
    { ITEM_PILHA,     { 530.0f, 170.0f }, 50.0f, false, MAPA_SALA_ZELADOR  },
};

/* Zero-inicializado: cabeca=NULL, tamanho=0 — lista vazia no início. */
static ListaItensChao itensChao;

static ListaElementos elementosChao;
static bool elementosInicializados = false;
static int  lixosNoSaco = 0;
#define MAX_LIXOS_SACO 5

/* ---------------------------------------------------------------
 * QuickSort e Spawn de Sujeiras/Lixos
 * --------------------------------------------------------------- */

typedef struct {
    TipoElemento tipo;
    Vector2 posicao;
    int mapaId;
} CandidatoElemento;

static void quickSortCandidatos(CandidatoElemento *arr, int low, int high) {
    if (low >= high) return;
    float pivot = arr[high].posicao.x;
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j].posicao.x <= pivot) {
            i++;
            CandidatoElemento tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
        }
    }
    CandidatoElemento tmp = arr[i + 1]; arr[i + 1] = arr[high]; arr[high] = tmp;
    int pi = i + 1;
    quickSortCandidatos(arr, low, pi - 1);
    quickSortCandidatos(arr, pi + 1, high);
}

static void SpawnElementos(void) {
    /* Bounds de spawn por mapa: x0,x1,y0,y1, numSujeiras, numLixos.
     * Saguão recebe apenas sujeiras (nl=0). */
    typedef struct { int mapaId; int x0,x1,y0,y1; int ns,nl; } Cfg;
    static const Cfg mapas[] = {
        { MAPA_SAGUAO,        200, 1720, 250, 830, 4, 0 },
        { MAPA_CORREDOR_1,    150, 1750, 150, 950, 3, 2 },
        { MAPA_SALA_1,        100, 1800, 500, 950, 3, 2 },
        { MAPA_CORREDOR_2,    150, 1750, 150, 950, 3, 2 },
        { MAPA_SALA_2,        100, 1800, 500, 950, 3, 2 },
        { MAPA_BANHEIRO_FEM,  250, 1600, 300, 850, 3, 1 },
        { MAPA_BANHEIRO_MASC, 250, 1600, 300, 850, 3, 1 },
        { MAPA_SALA_ZELADOR,  450, 1850, 450, 850, 3, 0 },
    };
    int nmapas = (int)(sizeof(mapas) / sizeof(mapas[0]));

    CandidatoElemento candidatos[60];
    int total = 0;
    for (int m = 0; m < nmapas; m++) {
        for (int i = 0; i < mapas[m].ns; i++) {
            candidatos[total].tipo        = ELEMENTO_SUJEIRA;
            candidatos[total].posicao.x   = (float)GetRandomValue(mapas[m].x0, mapas[m].x1);
            candidatos[total].posicao.y   = (float)GetRandomValue(mapas[m].y0, mapas[m].y1);
            candidatos[total].mapaId      = mapas[m].mapaId;
            total++;
        }
        for (int i = 0; i < mapas[m].nl; i++) {
            candidatos[total].tipo        = ELEMENTO_LIXO;
            candidatos[total].posicao.x   = (float)GetRandomValue(mapas[m].x0, mapas[m].x1);
            candidatos[total].posicao.y   = (float)GetRandomValue(mapas[m].y0, mapas[m].y1);
            candidatos[total].mapaId      = mapas[m].mapaId;
            total++;
        }
    }
    quickSortCandidatos(candidatos, 0, total - 1);
    for (int i = 0; i < total; i++)
        InserirElemento(&elementosChao, candidatos[i].tipo, candidatos[i].posicao, candidatos[i].mapaId);
}

TelaAtual UpdateGameplay(Personagem *player) {
    if (!elementosInicializados) {
        InicializarListaElementos(&elementosChao);
        SpawnElementos();
        elementosInicializados = true;
    }

    static Vector2 prevPosicao = { 960.0f, 960.0f };
    float raio = 20.0f;

    // Colisão sólida com poltronas e tela nas salas
    if (mapaAtual == MAPA_SALA_1 || mapaAtual == MAPA_SALA_2) {
        if (CheckCollisionCircleRec(player->posicao, raio, telaBloco)        ||
            CheckCollisionCircleRec(player->posicao, raio, poltronasEsq)     ||
            CheckCollisionCircleRec(player->posicao, raio, poltronasDirr)    ||
            CheckCollisionCircleRec(player->posicao, raio, poltronasCentEsq) ||
            CheckCollisionCircleRec(player->posicao, raio, poltronasCentDir)) {
            player->posicao = prevPosicao;
        }
    }

    if (mapaAtual == MAPA_BANHEIRO_FEM) {
        if (CheckCollisionCircleRec(player->posicao, raio, cabinesBanheiroFem) ||
            CheckCollisionCircleRec(player->posicao, raio, piasBanheiroFem)) {
            player->posicao = prevPosicao;
        }
    }

    if (mapaAtual == MAPA_BANHEIRO_MASC) {
        if (CheckCollisionCircleRec(player->posicao, raio, cabinesBanheiroMasc) ||
            CheckCollisionCircleRec(player->posicao, raio, piasBanheiroMasc)) {
            player->posicao = prevPosicao;
        }
    }

    if (mapaAtual == MAPA_SALA_ZELADOR) {
        if (CheckCollisionCircleRec(player->posicao, raio, armarioZelador) ||
            CheckCollisionCircleRec(player->posicao, raio, caixasZelador)) {
            player->posicao = prevPosicao;
        }
    }

    if (player->posicao.x < raio) {
        player->posicao.x = raio;
    }

    if (player->posicao.x > 1920.0f - raio) {
        player->posicao.x = 1920.0f - raio;
    }

    if (player->posicao.y < raio) {
        player->posicao.y = raio;
    }

    if (player->posicao.y > 1080.0f - raio) {
        player->posicao.y = 1080.0f - raio;
    }


    switch (mapaAtual) {

        case MAPA_SAGUAO:
            if (CheckCollisionCircleRec(player->posicao, raio, portaCorredor1)) {
                mapaAtual = MAPA_CORREDOR_1;
                player->posicao.x = 1750.0f;
                player->posicao.y = 540.0f;
            }
            else if (CheckCollisionCircleRec(player->posicao, raio, portaCorredor2)) {
                mapaAtual = MAPA_CORREDOR_2;
                player->posicao.x = 150.0f;
                player->posicao.y = 540.0f;
            }
            else if (CheckCollisionCircleRec(player->posicao, raio, portaSaida)) {
                player->posicao.x = 960.0f;
                player->posicao.y = 1050.0f;
                return TELA_ENTRADA;
            }
            else if (CheckCollisionCircleRec(player->posicao, raio, portaBanheiroFem)) {
                mapaAtual = MAPA_BANHEIRO_FEM;
                player->posicao.x = 960.0f;
                player->posicao.y = 540.0f;
            }
            else if (CheckCollisionCircleRec(player->posicao, raio, portaBanheiroMasc)) {
                mapaAtual = MAPA_BANHEIRO_MASC;
                player->posicao.x = 960.0f;
                player->posicao.y = 540.0f;
            }
            else if (CheckCollisionCircleRec(player->posicao, raio, portaZelador)) {
                mapaAtual = MAPA_SALA_ZELADOR;
                player->posicao.x = 960.0f;
                player->posicao.y = 700.0f;
            }
            break;


        case MAPA_CORREDOR_1:
            if (CheckCollisionCircleRec(player->posicao, raio, portaDireita)) {
                mapaAtual = MAPA_SAGUAO;
                player->posicao.x = 250.0f;
                player->posicao.y = 100.0f;
            }

            else if (CheckCollisionCircleRec(player->posicao, raio, portaEsquerda)) {
                mapaAtual = MAPA_SALA_1;
                player->posicao.x = 80.0f;
                player->posicao.y = 800.0f;
            }
            break;

   
        case MAPA_SALA_1:
            if (CheckCollisionCircleRec(player->posicao, raio, portaSaidaSala1)) {
                mapaAtual = MAPA_CORREDOR_1;
                player->posicao.x = 150.0f;
            }
            break;

     
        case MAPA_CORREDOR_2:
       
            if (CheckCollisionCircleRec(player->posicao, raio, portaDireita)) {
                mapaAtual = MAPA_SALA_2;
                player->posicao.x = 1840.0f;
                player->posicao.y = 800.0f;
            }
            
            else if (CheckCollisionCircleRec(player->posicao, raio, portaEsquerda)) {
                mapaAtual = MAPA_SAGUAO;
                player->posicao.x = 1650.0f; 
                player->posicao.y = 100.0f;
            }
            break;

     
        case MAPA_SALA_2:
            if (CheckCollisionCircleRec(player->posicao, raio, portaSaidaSala2)) {
                mapaAtual = MAPA_CORREDOR_2;
                player->posicao.x = 1750.0f;
            }
            break;

        case MAPA_BANHEIRO_FEM:
            if (CheckCollisionCircleRec(player->posicao, raio, portaSaidaQuarto)) {
                mapaAtual = MAPA_SAGUAO;
                player->posicao.x = 150.0f;
                player->posicao.y = 550.0f;
            }
            break;

        case MAPA_BANHEIRO_MASC:
            if (CheckCollisionCircleRec(player->posicao, raio, portaSaidaQuarto)) {
                mapaAtual = MAPA_SAGUAO;
                player->posicao.x = 1770.0f;
                player->posicao.y = 550.0f;
            }
            break;

        case MAPA_SALA_ZELADOR:
            if (CheckCollisionCircleRec(player->posicao, raio, portaSaidaQuarto)) {
                mapaAtual = MAPA_SAGUAO;
                player->posicao.x = 960.0f;
                player->posicao.y = 200.0f;
            }
            break;
    }

    if (IsKeyPressed(KEY_ONE))   TrocarSlotAtivo(&player->inventario, 0);
    if (IsKeyPressed(KEY_TWO))   TrocarSlotAtivo(&player->inventario, 1);
    if (IsKeyPressed(KEY_THREE)) TrocarSlotAtivo(&player->inventario, 2);

    // Clique esquerdo: vassoura limpa sujeira; saco coleta lixo (máx 5)
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && player->inventario.ativo->item != ITEM_VAZIO) {
        TipoItem ativo = player->inventario.ativo->item;
        if (ativo == ITEM_VASSOURA) {
            RemoverElementoProximo(&elementosChao, ELEMENTO_SUJEIRA, player->posicao, (int)mapaAtual, 80.0f);
        } else if (ativo == ITEM_SACO_LIXO && lixosNoSaco < MAX_LIXOS_SACO) {
            if (RemoverElementoProximo(&elementosChao, ELEMENTO_LIXO, player->posicao, (int)mapaAtual, 80.0f))
                lixosNoSaco++;
        }
    }

    // Q: larga item ativo no chão na posição do personagem
    if (IsKeyPressed(KEY_Q) && player->inventario.ativo->item != ITEM_VAZIO) {
        TipoItem tipoAtivo = player->inventario.ativo->item;
        int dadosItem = (tipoAtivo == ITEM_SACO_LIXO) ? lixosNoSaco : 0;
        RemoverItemAtivo(&player->inventario);
        LargarItemChao(&itensChao, tipoAtivo, player->posicao, (int)mapaAtual, dadosItem);
    }

    // flag para consumir o press de E uma única vez por frame
    bool pressionouE = IsKeyPressed(KEY_E);

    // Itens largados no chão têm prioridade; suportam troca quando inventário cheio
    ItemChao *proximo = ItemChaoProximo(&itensChao, player->posicao, (int)mapaAtual, 50.0f);
    if (proximo != NULL && pressionouE) {
        pressionouE = false;
        int dadosChao = proximo->dados;
        if (InventarioEstaCheio(&player->inventario)) {
            TipoItem tipoAtivo  = player->inventario.ativo->item;
            int      dadosAtivo = (tipoAtivo == ITEM_SACO_LIXO) ? lixosNoSaco : 0;
            RemoverItemAtivo(&player->inventario);
            TipoItem tipoChao = ColetarItemChao(&itensChao, player->posicao, (int)mapaAtual, 50.0f);
            AdicionarItem(&player->inventario, tipoChao);
            LargarItemChao(&itensChao, tipoAtivo, player->posicao, (int)mapaAtual, dadosAtivo);
            if (tipoChao == ITEM_SACO_LIXO) lixosNoSaco = dadosChao;
        } else {
            TipoItem tipoChao = ColetarItemChao(&itensChao, player->posicao, (int)mapaAtual, 50.0f);
            AdicionarItem(&player->inventario, tipoChao);
            if (tipoChao == ITEM_SACO_LIXO) lixosNoSaco = dadosChao;
        }
    }

    // Itens pré-colocados no mundo: coleta simples (sem troca)
    for (int i = 0; i < MAX_ITENS_MUNDO; i++) {
        if (itensMundo[i].coletado || itensMundo[i].mapa != mapaAtual) continue;
        float dx = player->posicao.x - itensMundo[i].posicao.x;
        float dy = player->posicao.y - itensMundo[i].posicao.y;
        bool perto = (dx * dx + dy * dy) <= (itensMundo[i].raioColeta * itensMundo[i].raioColeta);
        if (perto && pressionouE) {
            pressionouE = false;
            if (AdicionarItem(&player->inventario, itensMundo[i].tipo)) {
                itensMundo[i].coletado = true;
                if (itensMundo[i].tipo == ITEM_SACO_LIXO) lixosNoSaco = 0;
            }
        }
    }

    prevPosicao = player->posicao;
    return TELA_GAMEPLAY;
}

void DrawGameplay(Personagem player) {
    

    switch (mapaAtual) {
        
        case MAPA_SAGUAO:
            ClearBackground(DARKGRAY);
            DrawText("SAGUAO PRINCIPAL", 800, 50, 40, WHITE);
            DrawRectangleRec(portaCorredor1, MAROON);
            DrawRectangleRec(portaCorredor2, DARKBLUE);
            DrawRectangleRec(portaSaida, GOLD);
            DrawText("SAIDA", 910, 950, 30, DARKGRAY);
            DrawRectangleRec(portaBanheiroFem, PINK);
            DrawText("WC F", 12, 535, 20, WHITE);
            DrawRectangleRec(portaBanheiroMasc, SKYBLUE);
            DrawText("WC M", 1826, 535, 20, WHITE);
            DrawRectangleRec(portaZelador, BROWN);
            DrawText("ZELADOR", 880, 110, 18, WHITE);
            break;

        case MAPA_CORREDOR_1:
            ClearBackground(BLACK); 
            DrawText("CORREDOR 1", 850, 50, 40, MAROON);
            DrawRectangleRec(portaEsquerda, GRAY); 
            DrawRectangleRec(portaDireita, MAROON); 
            break;

        case MAPA_SALA_1:
            ClearBackground(DARKGRAY);

            // Palco (área caminhável superior)
            DrawRectangle(0, 0, 1920, 300, GRAY);

            // aqui será desenhado o PNG da tela
            DrawRectangleRec(telaBloco, RAYWHITE);

            // aqui será desenhado o PNG das poltronas
            DrawRectangleRec(poltronasEsq, BROWN);
            DrawRectangleRec(poltronasDirr, BROWN);
            DrawRectangleRec(poltronasCentEsq, BROWN);
            DrawRectangleRec(poltronasCentDir, BROWN);

            // Porta de saída — canto inferior esquerdo
            DrawRectangleRec(portaSaidaSala1, MAROON);
            break;

        case MAPA_CORREDOR_2:
            ClearBackground(BLACK); 
            DrawText("CORREDOR 2", 850, 50, 40, DARKBLUE);
            DrawRectangleRec(portaEsquerda, GRAY); 
            DrawRectangleRec(portaDireita, DARKBLUE); 
            break;

        case MAPA_SALA_2:
            ClearBackground(DARKGRAY);

            // Palco (área caminhável superior)
            DrawRectangle(0, 0, 1920, 300, GRAY);

            // aqui será desenhado o PNG da tela
            DrawRectangleRec(telaBloco, RAYWHITE);

            // aqui será desenhado o PNG das poltronas
            DrawRectangleRec(poltronasEsq, BROWN);
            DrawRectangleRec(poltronasDirr, BROWN);
            DrawRectangleRec(poltronasCentEsq, BROWN);
            DrawRectangleRec(poltronasCentDir, BROWN);

            // Porta de saída — canto inferior direito
            DrawRectangleRec(portaSaidaSala2, DARKBLUE);
            break;

        case MAPA_BANHEIRO_FEM:
            ClearBackground((Color){ 220, 180, 220, 255 });
            DrawText("BANHEIRO FEMININO", 700, 50, 40, WHITE);
            DrawRectangleRec(cabinesBanheiroFem, (Color){ 180, 140, 180, 255 });
            DrawText("CABINES", 55, 400, 20, WHITE);
            DrawRectangleRec(piasBanheiroFem, LIGHTGRAY);
            DrawText("PIAS", 1745, 160, 20, DARKGRAY);
            DrawRectangleRec(portaSaidaQuarto, GOLD);
            DrawText("SAIDA", 910, 950, 30, DARKGRAY);
            break;

        case MAPA_BANHEIRO_MASC:
            ClearBackground((Color){ 180, 210, 230, 255 });
            DrawText("BANHEIRO MASCULINO", 680, 50, 40, WHITE);
            DrawRectangleRec(cabinesBanheiroMasc, (Color){ 140, 170, 200, 255 });
            DrawText("CABINES", 1730, 400, 20, WHITE);
            DrawRectangleRec(piasBanheiroMasc, LIGHTGRAY);
            DrawText("PIAS", 80, 160, 20, DARKGRAY);
            DrawRectangleRec(portaSaidaQuarto, GOLD);
            DrawText("SAIDA", 910, 950, 30, DARKGRAY);
            break;

        case MAPA_SALA_ZELADOR:
            ClearBackground((Color){ 120, 100, 80, 255 });
            DrawText("SALA DO ZELADOR", 730, 50, 40, BEIGE);
            DrawRectangleRec(armarioZelador, (Color){ 80, 60, 40, 255 });
            DrawText("ARMARIO", 100, 145, 20, BEIGE);
            DrawRectangleRec(caixasZelador, (Color){ 160, 120, 70, 255 });
            DrawText("CAIXAS", 820, 260, 20, DARKBROWN);
            DrawRectangleRec(portaSaidaQuarto, GOLD);
            DrawText("SAIDA", 910, 950, 30, DARKGRAY);
            break;
    }


    // Itens pré-colocados: prompt só aparece quando há slot livre
    for (int i = 0; i < MAX_ITENS_MUNDO; i++) {
        if (itensMundo[i].coletado || itensMundo[i].mapa != mapaAtual) continue;
        Vector2 pos = itensMundo[i].posicao;
        DrawCircle((int)pos.x, (int)pos.y, 14.0f, BLACK);
        DrawCircle((int)pos.x, (int)pos.y, 12.0f, CorItem(itensMundo[i].tipo));
        DrawText(NomeItem(itensMundo[i].tipo), (int)pos.x - 20, (int)pos.y - 30, 16, WHITE);
        float dx = player.posicao.x - pos.x;
        float dy = player.posicao.y - pos.y;
        if ((dx * dx + dy * dy) <= (itensMundo[i].raioColeta * itensMundo[i].raioColeta) &&
            !InventarioEstaCheio(&player.inventario)) {
            DrawText(TextFormat("[E] Pegar %s", NomeItem(itensMundo[i].tipo)),
                     (int)pos.x - 80, (int)pos.y - 50, 18, YELLOW);
        }
    }

    // Itens largados no chão: percorre a lista simplesmente encadeada
    ItemChao *cur = itensChao.cabeca;
    while (cur != NULL) {
        if (cur->mapaId == (int)mapaAtual) {
            DrawCircle((int)cur->posicao.x, (int)cur->posicao.y, 14.0f, BLACK);
            DrawCircle((int)cur->posicao.x, (int)cur->posicao.y, 12.0f, CorItem(cur->tipo));
            DrawText(NomeItem(cur->tipo), (int)cur->posicao.x - 20, (int)cur->posicao.y - 30, 16, WHITE);
            float dx = player.posicao.x - cur->posicao.x;
            float dy = player.posicao.y - cur->posicao.y;
            if ((dx * dx + dy * dy) <= (50.0f * 50.0f)) {
                const char *prompt = InventarioEstaCheio(&player.inventario)
                    ? TextFormat("[E] Trocar por %s", NomeItem(cur->tipo))
                    : TextFormat("[E] Pegar %s",      NomeItem(cur->tipo));
                DrawText(prompt, (int)cur->posicao.x - 90, (int)cur->posicao.y - 50, 18, YELLOW);
            }
        }
        cur = cur->proximo;
    }

    // Elementos no chão (sujeiras e lixos) — lista circular encadeada
    if (elementosChao.cabeca != NULL) {
        TipoItem itemAtivo = player.inventario.ativo->item;
        NodoElemento *cur = elementosChao.cabeca;
        do {
            if (cur->mapaId == (int)mapaAtual) {
                Vector2 pos = cur->posicao;
                if (cur->tipo == ELEMENTO_SUJEIRA) {
                    DrawCircle((int)pos.x, (int)pos.y, 10.0f, (Color){ 150, 120, 80, 200 });
                    DrawRectangle((int)pos.x - 8, (int)pos.y - 3, 16, 6, (Color){ 120, 90, 50, 180 });
                } else {
                    DrawCircle((int)pos.x, (int)pos.y, 8.0f, ORANGE);
                    DrawText("Lixo", (int)pos.x - 12, (int)pos.y - 22, 13, ORANGE);
                }
                float dx = player.posicao.x - pos.x;
                float dy = player.posicao.y - pos.y;
                if ((dx * dx + dy * dy) <= (80.0f * 80.0f)) {
                    if (cur->tipo == ELEMENTO_SUJEIRA && itemAtivo == ITEM_VASSOURA)
                        DrawText("[CLIQUE] Limpar Sujeira", (int)pos.x - 90, (int)pos.y - 35, 16, YELLOW);
                    else if (cur->tipo == ELEMENTO_LIXO && itemAtivo == ITEM_SACO_LIXO) {
                        if (lixosNoSaco < MAX_LIXOS_SACO)
                            DrawText("[CLIQUE] Coletar Lixo", (int)pos.x - 80, (int)pos.y - 35, 16, YELLOW);
                        else
                            DrawText("Saco cheio!", (int)pos.x - 45, (int)pos.y - 35, 16, RED);
                    }
                }
            }
            cur = cur->proximo;
        } while (cur != elementosChao.cabeca);
    }

    DrawPersonagem(player);
    DrawInventario(player.inventario);

    if (player.inventario.ativo->item == ITEM_SACO_LIXO)
        DrawText(TextFormat("Lixo: %d/%d", lixosNoSaco, MAX_LIXOS_SACO),
                 20, 20, 22, lixosNoSaco >= MAX_LIXOS_SACO ? RED : WHITE);
}