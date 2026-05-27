#include "gameplay.h"
#include "item.h"
#include "grafo.h"
#include "expediente.h"
#include "audio.h"
#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>

static GrafoMapa grafo;
static bool grafoInicializado = false;

static IdSala salaAtual = SALA_SAGUAO;
static FilaInimigos filaInimigos;
static bool filaInicializada = false;
static bool aparicoesAtivadas = false;

static ListaExpediente expediente;
static bool expedienteInicializado = false;

static float tempoDecorrido = 0.0f;
static bool tempoComecou = false;

static bool perseguicaoAtiva = false;

static Rectangle telaBloco        = {  710.0f,  20.0f, 500.0f, 200.0f };
static Rectangle poltronasEsq     = {  100.0f, 300.0f, 360.0f, 480.0f };
static Rectangle poltronasDirr    = { 1460.0f, 300.0f, 360.0f, 480.0f };
static Rectangle poltronasCentEsq = {  560.0f, 300.0f, 300.0f, 480.0f };
static Rectangle poltronasCentDir = { 1060.0f, 300.0f, 300.0f, 480.0f };

static Rectangle cabinesBanheiroFem  = {   40.0f,  80.0f, 160.0f, 700.0f };
static Rectangle piasBanheiroFem     = { 1700.0f,  80.0f, 180.0f, 200.0f };
static Rectangle cabinesBanheiroMasc = { 1720.0f,  80.0f, 160.0f, 700.0f };
static Rectangle piasBanheiroMasc    = {   40.0f,  80.0f, 180.0f, 200.0f };

static Rectangle armarioZelador = {  40.0f,  40.0f, 350.0f, 250.0f };
static Rectangle caixasZelador  = { 600.0f, 150.0f, 700.0f, 250.0f };
static Rectangle depositoLixo   = { 1500.0f, 450.0f, 200.0f, 250.0f };

static Rectangle bilheteria = { 660.0f, 30.0f, 600.0f, 180.0f };

static Rectangle corredorParedeTopo  = { 0.0f,   0.0f, 1920.0f, 340.0f };
static Rectangle corredorParedeBaixo = { 0.0f, 740.0f, 1920.0f, 340.0f };

#define MAX_ITENS_MUNDO 8

typedef struct {
    TipoItem tipo;
    Vector2 posicao;
    float raioColeta;
    bool coletado;
    IdSala mapa;
} ItemMundo;

static ItemMundo itensMundo[MAX_ITENS_MUNDO] = {
    { ITEM_CHAVE,              {  960.0f, 240.0f }, 50.0f, false, SALA_SAGUAO  },
    { ITEM_VASSOURA,           {  450.0f,  80.0f }, 50.0f, false, SALA_ZELADOR },
    { ITEM_SACO_LIXO,          { 1700.0f, 250.0f }, 50.0f, false, SALA_ZELADOR },
    { ITEM_SACO_LIXO,          { 1800.0f, 800.0f }, 50.0f, false, SALA_ZELADOR },
    { ITEM_CHAVE_BANHEIRO_FEM, {  250.0f, 500.0f }, 50.0f, false, SALA_ZELADOR },
    { ITEM_CHAVE_BANHEIRO_MASC,{  500.0f, 500.0f }, 50.0f, false, SALA_ZELADOR },
    { ITEM_CHAVE_SALA1,        {  500.0f, 800.0f }, 50.0f, false, SALA_ZELADOR },
    { ITEM_CHAVE_SALA2,        { 1400.0f, 800.0f }, 50.0f, false, SALA_ZELADOR },
};

static ListaItensChao itensChao;
static ListaElementos elementosChao;
static bool elementosInicializados = false;
static int  lixosNoSaco = 0;
#define MAX_LIXOS_SACO 5

static int portaBloqueadaMsgTimer = 0;
static int saidaBloqueadaMsgTimer = 0;

static int  portaChaveMsgTimer = 0;
static bool portaChavePrompt   = false;

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
    typedef struct { int mapaId; int x0,x1,y0,y1; int ns,nl; } Cfg;
    static const Cfg mapas[] = {
        { SALA_SAGUAO,        200, 1680, 280, 820, 4, 0 },
        { SALA_CORREDOR_1,    200, 1700, 380, 700, 3, 2 },
        { SALA_CINEMA_1,      150, 1700, 800, 950, 3, 2 },
        { SALA_CORREDOR_2,    200, 1700, 380, 700, 3, 2 },
        { SALA_CINEMA_2,      150, 1700, 800, 950, 3, 2 },
        { SALA_BANHEIRO_FEM,  300, 1500, 350, 750, 3, 1 },
        { SALA_BANHEIRO_MASC, 300, 1500, 350, 750, 3, 1 },
        { SALA_ZELADOR,       500, 1750, 500, 800, 3, 0 },
    };
    int nmapas = (int)(sizeof(mapas) / sizeof(mapas[0]));

    CandidatoElemento candidatos[60];
    int total = 0;
    for (int m = 0; m < nmapas; m++) {
        for (int i = 0; i < mapas[m].ns; i++) {
            candidatos[total].tipo      = ELEMENTO_SUJEIRA;
            candidatos[total].posicao.x = (float)GetRandomValue(mapas[m].x0, mapas[m].x1);
            candidatos[total].posicao.y = (float)GetRandomValue(mapas[m].y0, mapas[m].y1);
            candidatos[total].mapaId    = mapas[m].mapaId;
            total++;
        }
        for (int i = 0; i < mapas[m].nl; i++) {
            candidatos[total].tipo      = ELEMENTO_LIXO;
            candidatos[total].posicao.x = (float)GetRandomValue(mapas[m].x0, mapas[m].x1);
            candidatos[total].posicao.y = (float)GetRandomValue(mapas[m].y0, mapas[m].y1);
            candidatos[total].mapaId    = mapas[m].mapaId;
            total++;
        }
    }
    quickSortCandidatos(candidatos, 0, total - 1);
    for (int i = 0; i < total; i++)
        InserirElemento(&elementosChao, candidatos[i].tipo, candidatos[i].posicao, candidatos[i].mapaId);
}

static bool LimpezaCompleta(void) {
    return elementosChao.cabeca == NULL;
}

typedef struct { int x0, y0, x1, y1; } AreaSpawn;

static AreaSpawn areaSpawnDaSala(IdSala sala) {
    switch (sala) {
        case SALA_SAGUAO:        return (AreaSpawn){ 200, 280, 1680, 820 };
        case SALA_CORREDOR_1:    return (AreaSpawn){ 200, 380, 1700, 700 };
        case SALA_CORREDOR_2:    return (AreaSpawn){ 200, 380, 1700, 700 };
        case SALA_CINEMA_1:      return (AreaSpawn){ 150, 800, 1700, 950 };
        case SALA_CINEMA_2:      return (AreaSpawn){ 150, 800, 1700, 950 };
        case SALA_BANHEIRO_FEM:  return (AreaSpawn){ 300, 350, 1500, 750 };
        case SALA_BANHEIRO_MASC: return (AreaSpawn){ 300, 350, 1500, 750 };
        case SALA_ZELADOR:       return (AreaSpawn){ 500, 500, 1750, 800 };
        default:                 return (AreaSpawn){ 200, 200, 1700, 880 };
    }
}

float GameplayTempoDecorrido(void) {
    return tempoDecorrido;
}

static void EnfileirarInimigosPadrao(void) {
    int tipos[5] = {
        INIMIGO_STALKER, INIMIGO_STALKER, INIMIGO_STALKER,
        INIMIGO_WITCH,   INIMIGO_WITCH
    };

    for (int i = 4; i > 0; i--) {
        int j = GetRandomValue(0, i);
        int tmp = tipos[i]; tipos[i] = tipos[j]; tipos[j] = tmp;
    }

    for (int i = 0; i < 5; i++) {
        Inimigo inimigo;
        inimigo.posicao.x  = (float)GetRandomValue(200, 1720);
        inimigo.posicao.y  = (float)GetRandomValue(200, 880);
        inimigo.raio       = 20.0f;
        inimigo.ativo      = false;
        inimigo.tempoVida  = 0.0f;
        inimigo.tipo       = tipos[i];
        inimigo.velocidade = (tipos[i] == INIMIGO_STALKER) ? 3.25f : 5.2f;

        EnfileirarInimigo(&filaInimigos, inimigo);
    }
}

void ResetarGameplay(Personagem *player) {
    if (grafoInicializado) {
        LiberarGrafo(&grafo);
        grafoInicializado = false;
    }
    if (expedienteInicializado) {
        LiberarExpediente(&expediente);
        expedienteInicializado = false;
    }
    if (elementosChao.cabeca != NULL) {
        NodoElemento *cabeca = elementosChao.cabeca;
        NodoElemento *cur = cabeca;
        do {
            NodoElemento *next = cur->proximo;
            free(cur);
            cur = next;
        } while (cur != cabeca);
        elementosChao.cabeca = NULL;
        elementosChao.tamanho = 0;
    }
    elementosInicializados = false;

    while (itensChao.cabeca != NULL) {
        ItemChao *tmp = itensChao.cabeca;
        itensChao.cabeca = tmp->proximo;
        free(tmp);
    }
    itensChao.tamanho = 0;

    filaInicializada = false;

    for (int i = 0; i < MAX_ITENS_MUNDO; i++) {
        itensMundo[i].coletado = false;
    }

    salaAtual              = SALA_SAGUAO;
    aparicoesAtivadas      = false;
    perseguicaoAtiva       = false;
    tempoComecou           = false;
    tempoDecorrido         = 0.0f;
    lixosNoSaco            = 0;
    portaBloqueadaMsgTimer = 0;
    saidaBloqueadaMsgTimer = 0;
    portaChaveMsgTimer     = 0;
    portaChavePrompt       = false;

    InicializarPersonagem(player);
}

static void ResetarInimigos(void) {
    filaInimigos.inicio  = 0;
    filaInimigos.fim     = 0;
    filaInimigos.tamanho = 0;
    for (int i = 0; i < FILA_INIMIGOS_MAX; i++) {
        filaInimigos.inimigos[i].ativo = false;
    }
    filaInimigos.tempoProximaAparicao = filaInimigos.intervaloAparicao;
    EnfileirarInimigosPadrao();
}

TelaAtual UpdateGameplay(Personagem *player) {
    if (!grafoInicializado) {
        InicializarGrafo(&grafo);
        grafoInicializado = true;
    }

    if (!expedienteInicializado) {
        IniciarExpediente(&expediente);
        expedienteInicializado = true;
    }

    if (!tempoComecou) {
        tempoDecorrido = 0.0f;
        tempoComecou = true;
    }
    tempoDecorrido += GetFrameTime();

    if (!elementosInicializados) {
        InicializarListaElementos(&elementosChao);
        SpawnElementos();
        elementosInicializados = true;
    }

    if (!filaInicializada) {
        InicializarFilaInimigos(&filaInimigos, 3.0f);
        EnfileirarInimigosPadrao();
        filaInicializada = true;
    }

    AreaSpawn area = areaSpawnDaSala(salaAtual);
    AtualizarFilaInimigos(&filaInimigos, player->posicao,
                          area.x0, area.y0, area.x1, area.y1);
    AtualizarMovimentoInimigos(&filaInimigos, player->posicao);

    bool algumAtivo = false;
    for (int i = 0; i < FILA_INIMIGOS_MAX; i++) {
        if (filaInimigos.inimigos[i].ativo) { algumAtivo = true; break; }
    }
    if (algumAtivo != perseguicaoAtiva) {
        perseguicaoAtiva = algumAtivo;
        if (perseguicaoAtiva) TocarMusicaPerseguicao();
        else                  TocarMusicaGame();
    }

    if (ColisaoComInimigos(&filaInimigos, player->posicao, 20.0f)) {
        return TELA_GAME_OVER;
    }

    AtualizarExpediente(&expediente, GetFrameTime());
    if (VerificarVitoria(&expediente)) {
        return LimpezaCompleta() ? TELA_NOME : TELA_GAME_OVER;
    }

    if (!aparicoesAtivadas &&
        expediente.atual != NULL && expediente.atual->hora == 0) {
        filaInimigos.tempoProximaAparicao = (float)GetRandomValue(10, 30) / 10.0f;
        AtivarAparicaoInimigos(&filaInimigos);
        aparicoesAtivadas = true;
    }

    static Vector2 prevPosicao = { 960.0f, 960.0f };
    float raio = 20.0f;

    if (salaAtual == SALA_CINEMA_1 || salaAtual == SALA_CINEMA_2) {
        if (CheckCollisionCircleRec(player->posicao, raio, telaBloco)        ||
            CheckCollisionCircleRec(player->posicao, raio, poltronasEsq)     ||
            CheckCollisionCircleRec(player->posicao, raio, poltronasDirr)    ||
            CheckCollisionCircleRec(player->posicao, raio, poltronasCentEsq) ||
            CheckCollisionCircleRec(player->posicao, raio, poltronasCentDir))
            player->posicao = prevPosicao;
    }
    if (salaAtual == SALA_BANHEIRO_FEM) {
        if (CheckCollisionCircleRec(player->posicao, raio, cabinesBanheiroFem) ||
            CheckCollisionCircleRec(player->posicao, raio, piasBanheiroFem))
            player->posicao = prevPosicao;
    }
    if (salaAtual == SALA_BANHEIRO_MASC) {
        if (CheckCollisionCircleRec(player->posicao, raio, cabinesBanheiroMasc) ||
            CheckCollisionCircleRec(player->posicao, raio, piasBanheiroMasc))
            player->posicao = prevPosicao;
    }
    if (salaAtual == SALA_ZELADOR) {
        if (CheckCollisionCircleRec(player->posicao, raio, armarioZelador) ||
            CheckCollisionCircleRec(player->posicao, raio, caixasZelador)   ||
            CheckCollisionCircleRec(player->posicao, raio, depositoLixo))
            player->posicao = prevPosicao;
    }
    if (salaAtual == SALA_SAGUAO) {
        if (CheckCollisionCircleRec(player->posicao, raio, bilheteria))
            player->posicao = prevPosicao;
    }
    if (salaAtual == SALA_CORREDOR_1 || salaAtual == SALA_CORREDOR_2) {
        if (CheckCollisionCircleRec(player->posicao, raio, corredorParedeTopo) ||
            CheckCollisionCircleRec(player->posicao, raio, corredorParedeBaixo))
            player->posicao = prevPosicao;
    }

    if (player->posicao.x < raio)           player->posicao.x = raio;
    if (player->posicao.x > 1920.0f - raio) player->posicao.x = 1920.0f - raio;
    if (player->posicao.y < raio)           player->posicao.y = raio;
    if (player->posicao.y > 1080.0f - raio) player->posicao.y = 1080.0f - raio;

    if (portaBloqueadaMsgTimer > 0) portaBloqueadaMsgTimer--;
    if (saidaBloqueadaMsgTimer > 0) saidaBloqueadaMsgTimer--;
    if (portaChaveMsgTimer     > 0) portaChaveMsgTimer--;
    portaChavePrompt = false;

    bool clicouEsquerdo = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    Aresta *porta = VerificarColisaoPorta(&grafo, salaAtual, player->posicao, raio);
    if (porta) {
        if (porta->bloqueada) {
            if (BuscarItem(&player->inventario, porta->chaveNecessaria)) {
                portaChavePrompt   = true;
                portaChaveMsgTimer = 5;
                if (clicouEsquerdo) {
                    SlotInventario *c = player->inventario.cabeca;
                    while (c) {
                        if (c->item == porta->chaveNecessaria) {
                            TrocarSlotAtivo(&player->inventario, c->indice);
                            break;
                        }
                        c = c->proximo;
                    }
                    RemoverItemAtivo(&player->inventario);
                    DesbloquearAcesso(&grafo, salaAtual, porta->destino);
                    clicouEsquerdo = false;
                }
            } else {
                portaBloqueadaMsgTimer = 120;
            }
        } else {
            if (porta->destino == SALA_SAIDA) {
                if (LimpezaCompleta()) {
                    IrParaSala(&salaAtual, &player->posicao, porta);
                    return TELA_NOME;
                } else {
                    saidaBloqueadaMsgTimer = 120;
                }
            } else {
                IrParaSala(&salaAtual, &player->posicao, porta);
                ResetarInimigos();
            }
        }
    }

    if (IsKeyPressed(KEY_ONE))   TrocarSlotAtivo(&player->inventario, 0);
    if (IsKeyPressed(KEY_TWO))   TrocarSlotAtivo(&player->inventario, 1);
    if (IsKeyPressed(KEY_THREE)) TrocarSlotAtivo(&player->inventario, 2);

    float scroll = GetMouseWheelMove();
    if (scroll != 0.0f) {
        int novoIdx = player->inventario.ativo->indice;
        if (scroll > 0.0f) novoIdx = (novoIdx - 1 + INVENTARIO_SLOTS) % INVENTARIO_SLOTS;
        else               novoIdx = (novoIdx + 1) % INVENTARIO_SLOTS;
        TrocarSlotAtivo(&player->inventario, novoIdx);
    }

    if (clicouEsquerdo && player->inventario.ativo->item != ITEM_VAZIO) {
        TipoItem ativo = player->inventario.ativo->item;
        if (ativo == ITEM_SACO_LIXO && salaAtual == SALA_ZELADOR &&
            lixosNoSaco >= MAX_LIXOS_SACO &&
            CheckCollisionCircleRec(player->posicao, 80.0f, depositoLixo)) {
            RemoverItemAtivo(&player->inventario);
            lixosNoSaco = 0;
        } else if (ativo == ITEM_VASSOURA) {
            RemoverElementoProximo(&elementosChao, ELEMENTO_SUJEIRA, player->posicao, (int)salaAtual, 80.0f);
        } else if (ativo == ITEM_SACO_LIXO && lixosNoSaco < MAX_LIXOS_SACO) {
            if (RemoverElementoProximo(&elementosChao, ELEMENTO_LIXO, player->posicao, (int)salaAtual, 80.0f))
                lixosNoSaco++;
        }
    }

    if (IsKeyPressed(KEY_Q) && player->inventario.ativo->item != ITEM_VAZIO) {
        TipoItem tipoAtivo = player->inventario.ativo->item;
        int dadosItem = (tipoAtivo == ITEM_SACO_LIXO) ? lixosNoSaco : 0;
        RemoverItemAtivo(&player->inventario);
        LargarItemChao(&itensChao, tipoAtivo, player->posicao, (int)salaAtual, dadosItem);
    }

    bool pressionouE = IsKeyPressed(KEY_E);

    ItemChao *proximo = ItemChaoProximo(&itensChao, player->posicao, (int)salaAtual, 50.0f);
    if (proximo != NULL && pressionouE) {
        pressionouE = false;
        int dadosChao = proximo->dados;
        if (InventarioEstaCheio(&player->inventario)) {
            TipoItem tipoAtivo  = player->inventario.ativo->item;
            int      dadosAtivo = (tipoAtivo == ITEM_SACO_LIXO) ? lixosNoSaco : 0;
            RemoverItemAtivo(&player->inventario);
            TipoItem tipoChao = ColetarItemChao(&itensChao, player->posicao, (int)salaAtual, 50.0f);
            AdicionarItem(&player->inventario, tipoChao);
            LargarItemChao(&itensChao, tipoAtivo, player->posicao, (int)salaAtual, dadosAtivo);
            if (tipoChao == ITEM_SACO_LIXO) lixosNoSaco = dadosChao;
        } else {
            TipoItem tipoChao = ColetarItemChao(&itensChao, player->posicao, (int)salaAtual, 50.0f);
            AdicionarItem(&player->inventario, tipoChao);
            if (tipoChao == ITEM_SACO_LIXO) lixosNoSaco = dadosChao;
        }
    }

    for (int i = 0; i < MAX_ITENS_MUNDO; i++) {
        if (itensMundo[i].coletado || itensMundo[i].mapa != salaAtual) continue;
        float dx = player->posicao.x - itensMundo[i].posicao.x;
        float dy = player->posicao.y - itensMundo[i].posicao.y;
        bool perto = (dx * dx + dy * dy) <= (itensMundo[i].raioColeta * itensMundo[i].raioColeta);
        if (perto && pressionouE) {
            pressionouE = false;
            if (AdicionarItem(&player->inventario, itensMundo[i].tipo)) {
                itensMundo[i].coletado = true;
                if (itensMundo[i].tipo == ITEM_SACO_LIXO) lixosNoSaco = 0;

                if (!aparicoesAtivadas && itensMundo[i].tipo == ITEM_CHAVE) {
                    filaInimigos.tempoProximaAparicao = (float)GetRandomValue(10, 30) / 10.0f;
                    AtivarAparicaoInimigos(&filaInimigos);
                    aparicoesAtivadas = true;
                }
            }
        }
    }

    prevPosicao = player->posicao;
    return TELA_GAMEPLAY;
}

static void DesenharPortasDaSala(IdSala sala) {
    Aresta *cur = grafo.nos[sala].adjacencias;
    while (cur) {
        Color cor;
        switch (cur->destino) {
            case SALA_CORREDOR_1:    cor = MAROON;   break;
            case SALA_CORREDOR_2:    cor = DARKBLUE; break;
            case SALA_BANHEIRO_FEM:  cor = PINK;     break;
            case SALA_BANHEIRO_MASC: cor = SKYBLUE;  break;
            case SALA_ZELADOR:       cor = BROWN;    break;
            case SALA_CINEMA_1:      cor = MAROON;   break;
            case SALA_CINEMA_2:      cor = DARKBLUE; break;
            case SALA_SAIDA:         cor = GOLD;     break;
            case SALA_SAGUAO:        cor = GRAY;     break;
            default:                 cor = LIGHTGRAY;break;
        }
        DrawRectangleRec(cur->gatilho, cor);
        cur = cur->proxima;
    }
}

void DrawGameplay(Personagem player) {
    switch (salaAtual) {
        case SALA_SAGUAO:
            ClearBackground(DARKGRAY);
            DesenharPortasDaSala(salaAtual);
            DrawRectangleRec(bilheteria, (Color){ 120, 80, 40, 255 });
            DrawText("BILHETERIA", 800, 100, 32, GOLD);
            DrawText("SAIDA",     910,  950, 30, DARKGRAY);
            DrawText("COR 1",      20,   90, 20, WHITE);
            DrawText("COR 2",    1750,   90, 20, WHITE);
            DrawText("WC F",       12,  535, 20, WHITE);
            DrawText("WC M",     1826,  535, 20, WHITE);
            DrawText("ZELADOR",  1730,  940, 18, WHITE);
            break;
        case SALA_CORREDOR_1:
            ClearBackground(BLACK);
            DrawRectangleRec(corredorParedeTopo, DARKGRAY);
            DrawRectangleRec(corredorParedeBaixo, DARKGRAY);
            DesenharPortasDaSala(salaAtual);
            DrawText("CORREDOR 1", 850, 130, 40, MAROON);
            break;
        case SALA_CINEMA_1:
            ClearBackground(DARKGRAY);
            DrawRectangle(0, 0, 1920, 300, GRAY);
            DrawRectangleRec(telaBloco, RAYWHITE);
            DrawRectangleRec(poltronasEsq, BROWN);
            DrawRectangleRec(poltronasDirr, BROWN);
            DrawRectangleRec(poltronasCentEsq, BROWN);
            DrawRectangleRec(poltronasCentDir, BROWN);
            DesenharPortasDaSala(salaAtual);
            DrawText("SAIDA", 1830, 940, 22, WHITE);
            break;
        case SALA_CORREDOR_2:
            ClearBackground(BLACK);
            DrawRectangleRec(corredorParedeTopo, DARKGRAY);
            DrawRectangleRec(corredorParedeBaixo, DARKGRAY);
            DesenharPortasDaSala(salaAtual);
            DrawText("CORREDOR 2", 850, 130, 40, DARKBLUE);
            break;
        case SALA_CINEMA_2:
            ClearBackground(DARKGRAY);
            DrawRectangle(0, 0, 1920, 300, GRAY);
            DrawRectangleRec(telaBloco, RAYWHITE);
            DrawRectangleRec(poltronasEsq, BROWN);
            DrawRectangleRec(poltronasDirr, BROWN);
            DrawRectangleRec(poltronasCentEsq, BROWN);
            DrawRectangleRec(poltronasCentDir, BROWN);
            DesenharPortasDaSala(salaAtual);
            DrawText("SAIDA", 10, 940, 22, WHITE);
            break;
        case SALA_BANHEIRO_FEM:
            ClearBackground((Color){ 220, 180, 220, 255 });
            DrawRectangleRec(cabinesBanheiroFem, (Color){ 180, 140, 180, 255 });
            DrawRectangleRec(piasBanheiroFem, LIGHTGRAY);
            DesenharPortasDaSala(salaAtual);
            DrawText("BANHEIRO FEMININO", 700, 50, 40, WHITE);
            DrawText("CABINES", 55, 400, 20, WHITE);
            DrawText("PIAS", 1745, 160, 20, DARKGRAY);
            DrawText("SAIDA", 1830, 530, 22, WHITE);
            break;
        case SALA_BANHEIRO_MASC:
            ClearBackground((Color){ 180, 210, 230, 255 });
            DrawRectangleRec(cabinesBanheiroMasc, (Color){ 140, 170, 200, 255 });
            DrawRectangleRec(piasBanheiroMasc, LIGHTGRAY);
            DesenharPortasDaSala(salaAtual);
            DrawText("BANHEIRO MASCULINO", 680, 50, 40, WHITE);
            DrawText("CABINES", 1730, 400, 20, WHITE);
            DrawText("PIAS", 80, 160, 20, DARKGRAY);
            DrawText("SAIDA", 10, 530, 22, WHITE);
            break;
        case SALA_ZELADOR:
            ClearBackground((Color){ 120, 100, 80, 255 });
            DrawRectangleRec(armarioZelador, (Color){ 80, 60, 40, 255 });
            DrawRectangleRec(caixasZelador, (Color){ 160, 120, 70, 255 });
            DrawRectangleRec(depositoLixo, (Color){ 60, 80, 60, 255 });
            DesenharPortasDaSala(salaAtual);
            DrawText("SALA DO ZELADOR", 730, 50, 40, BEIGE);
            DrawText("ARMARIO", 100, 145, 20, BEIGE);
            DrawText("CAIXAS", 820, 260, 20, DARKBROWN);
            DrawText("LIXEIRA", 1530, 560, 28, WHITE);
            DrawText("SAIDA", 910, 950, 30, DARKGRAY);
            break;
        default:
            ClearBackground(BLACK);
            DesenharPortasDaSala(salaAtual);
            break;
    }

    for (int i = 0; i < MAX_ITENS_MUNDO; i++) {
        if (itensMundo[i].coletado || itensMundo[i].mapa != salaAtual) continue;
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

    ItemChao *curItem = itensChao.cabeca;
    while (curItem != NULL) {
        if (curItem->mapaId == (int)salaAtual) {
            DrawCircle((int)curItem->posicao.x, (int)curItem->posicao.y, 14.0f, BLACK);
            DrawCircle((int)curItem->posicao.x, (int)curItem->posicao.y, 12.0f, CorItem(curItem->tipo));
            DrawText(NomeItem(curItem->tipo), (int)curItem->posicao.x - 20, (int)curItem->posicao.y - 30, 16, WHITE);
            float dx = player.posicao.x - curItem->posicao.x;
            float dy = player.posicao.y - curItem->posicao.y;
            if ((dx * dx + dy * dy) <= (50.0f * 50.0f)) {
                const char *prompt = InventarioEstaCheio(&player.inventario)
                    ? TextFormat("[E] Trocar por %s", NomeItem(curItem->tipo))
                    : TextFormat("[E] Pegar %s",      NomeItem(curItem->tipo));
                DrawText(prompt, (int)curItem->posicao.x - 90, (int)curItem->posicao.y - 50, 18, YELLOW);
            }
        }
        curItem = curItem->proximo;
    }

    if (elementosChao.cabeca != NULL) {
        TipoItem itemAtivo = player.inventario.ativo->item;
        NodoElemento *curE = elementosChao.cabeca;
        do {
            if (curE->mapaId == (int)salaAtual) {
                Vector2 pos = curE->posicao;
                if (curE->tipo == ELEMENTO_SUJEIRA) {
                    DrawCircle((int)pos.x, (int)pos.y, 10.0f, (Color){ 150, 120, 80, 200 });
                    DrawRectangle((int)pos.x - 8, (int)pos.y - 3, 16, 6, (Color){ 120, 90, 50, 180 });
                } else {
                    DrawCircle((int)pos.x, (int)pos.y, 8.0f, ORANGE);
                    DrawText("Lixo", (int)pos.x - 12, (int)pos.y - 22, 13, ORANGE);
                }
                float dx = player.posicao.x - pos.x;
                float dy = player.posicao.y - pos.y;
                if ((dx * dx + dy * dy) <= (80.0f * 80.0f)) {
                    if (curE->tipo == ELEMENTO_SUJEIRA && itemAtivo == ITEM_VASSOURA)
                        DrawText("[CLIQUE] Limpar Sujeira", (int)pos.x - 90, (int)pos.y - 35, 16, YELLOW);
                    else if (curE->tipo == ELEMENTO_LIXO && itemAtivo == ITEM_SACO_LIXO) {
                        if (lixosNoSaco < MAX_LIXOS_SACO)
                            DrawText("[CLIQUE] Coletar Lixo", (int)pos.x - 80, (int)pos.y - 35, 16, YELLOW);
                        else
                            DrawText("Saco cheio!", (int)pos.x - 45, (int)pos.y - 35, 16, RED);
                    }
                }
            }
            curE = curE->proximo;
        } while (curE != elementosChao.cabeca);
    }

    DrawPersonagem(player);
    DrawInventario(player.inventario);
    DrawFilaInimigos(&filaInimigos);

    const NoTurno *turno = TurnoAtual(&expediente);
    if (turno != NULL) {
        DrawText(turno->rotulo, 20, 60, 28, SKYBLUE);
    }

    if (player.inventario.ativo->item == ITEM_SACO_LIXO)
        DrawText(TextFormat("Lixo: %d/%d", lixosNoSaco, MAX_LIXOS_SACO),
                 20, 20, 22, lixosNoSaco >= MAX_LIXOS_SACO ? RED : WHITE);

    if (salaAtual == SALA_ZELADOR &&
        player.inventario.ativo->item == ITEM_SACO_LIXO &&
        CheckCollisionCircleRec(player.posicao, 80.0f, depositoLixo)) {
        if (lixosNoSaco >= MAX_LIXOS_SACO)
            DrawText("[CLIQUE] Deixar saco aqui", 1410, 415, 22, YELLOW);
        else
            DrawText("Saco precisa estar cheio", 1420, 415, 22, RED);
    }

    if (portaChavePrompt || portaChaveMsgTimer > 0) {
        DrawRectangle(560, 490, 800, 100, Fade(BLACK, 0.75f));
        DrawText("[CLIQUE] Usar chave para abrir", 600, 525, 28, YELLOW);
    }

    if (portaBloqueadaMsgTimer > 0) {
        DrawRectangle(560, 490, 800, 100, Fade(BLACK, 0.75f));
        DrawText("Trancado. Voce precisa de uma chave.", 580, 525, 28, RED);
    }

    if (saidaBloqueadaMsgTimer > 0) {
        DrawRectangle(440, 490, 1040, 100, Fade(BLACK, 0.75f));
        DrawText("Termine a limpeza antes de sair!", 510, 525, 32, RED);
    }

    int sujeirasRestantes = 0, lixosRestantes = 0;
    if (elementosChao.cabeca != NULL) {
        NodoElemento *cur = elementosChao.cabeca;
        do {
            if (cur->tipo == ELEMENTO_SUJEIRA) sujeirasRestantes++;
            else if (cur->tipo == ELEMENTO_LIXO) lixosRestantes++;
            cur = cur->proximo;
        } while (cur != elementosChao.cabeca);
    }
    DrawText(TextFormat("Sujeira: %d  |  Lixo: %d", sujeirasRestantes, lixosRestantes),
             20, 95, 22, LIGHTGRAY);
}
