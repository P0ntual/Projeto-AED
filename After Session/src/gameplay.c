#include "gameplay.h"
#include "item.h"
#include "grafo.h"
#include "expediente.h"
#include "audio.h"
#include "fonte.h"
#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>

static Texture2D chaoTexture;
static bool chaoCarregado = false;

static Texture2D chaoBanheiroTexture;
static bool chaoBanheiroCarregado = false;

static Texture2D chaoSalaTexture;
static bool chaoSalaCarregado = false;

static Texture2D bilheteriaTexture;
static bool bilheteriaCarregada = false;

static Texture2D sacoLixoTexture;
static bool sacoLixoCarregado = false;

static Texture2D sujeiraTexture;
static bool sujeiraCarregada = false;

static Texture2D armarioZeladorTexture;
static bool armarioZeladorCarregado = false;

static Texture2D lixeiroTexture;
static bool lixeiroCarregado = false;

static Texture2D caixaTexture;
static bool caixaCarregada = false;

static Texture2D chaveTexture;
static bool chaveCarregada = false;

static Texture2D cartazesTexture;
static bool cartazesCarregado = false;

static Texture2D portaTopdownTexture;
static bool portasCarregadas = false;

static Texture2D espelhosTexture;
static bool espelhosCarregado = false;

static Texture2D pipocaTexture;
static Texture2D pepiBlackTexture;
static bool sujeirasCarregadas = false;

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

static Rectangle armarioZelador = {  40.0f,  40.0f, 220.0f, 250.0f };
static Rectangle caixasZelador  = { 580.0f, 180.0f, 280.0f, 176.0f };
static Rectangle depositoLixo   = {  280.0f,  80.0f, 120.0f, 140.0f };

static Rectangle bilheteria = { 860.0f, 150.0f, 200.0f, 230.0f };

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
    { ITEM_CHAVE,              {  400.0f, 500.0f }, 50.0f, false, SALA_SAGUAO  },
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
        inimigo.raio       = 40.0f;
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
    if (!chaoCarregado) {
        chaoTexture = LoadTexture("assets/images/corredor/chao corredor.png");
        chaoCarregado = true;
    }

    if (!chaoBanheiroCarregado) {
        chaoBanheiroTexture = LoadTexture("assets/images/banheiros/chao banheiro.png");
        chaoBanheiroCarregado = true;
    }

    if (!chaoSalaCarregado) {
        chaoSalaTexture = LoadTexture("assets/images/sala entrada/sala.png");
        chaoSalaCarregado = true;
    }

    if (!bilheteriaCarregada) {
        bilheteriaTexture = LoadTexture("assets/images/sala entrada/bilheteria.png");
        bilheteriaCarregada = true;
    }

    if (!sacoLixoCarregado) {
        sacoLixoTexture = LoadTexture("assets/images/itens/saco de lixo.png");
        sacoLixoCarregado = true;
    }

    if (!sujeiraCarregada) {
        sujeiraTexture = LoadTexture("assets/images/itens/sujeira.png");
        sujeiraCarregada = true;
    }

    if (!armarioZeladorCarregado) {
        armarioZeladorTexture = LoadTexture("assets/images/sala zelador/armario zelador.png");
        armarioZeladorCarregado = true;
    }

    if (!lixeiroCarregado) {
        lixeiroTexture = LoadTexture("assets/images/sala zelador/lixeiro.png");
        lixeiroCarregado = true;
    }

    if (!caixaCarregada) {
        caixaTexture = LoadTexture("assets/images/sala zelador/caixa.png");
        caixaCarregada = true;
    }

    if (!sujeirasCarregadas) {
        pipocaTexture    = LoadTexture("assets/images/itens/pipoca.png");
        pepiBlackTexture = LoadTexture("assets/images/itens/pepi black.png");
        sujeirasCarregadas = true;
    }

    if (!chaveCarregada) {
        chaveTexture = LoadTexture("assets/images/itens/chaves wc.png");
        chaveCarregada = true;
    }

    if (!cartazesCarregado) {
        cartazesTexture = LoadTexture("assets/images/corredor/cartazes corredor.png");
        cartazesCarregado = true;
    }

    if (!portasCarregadas) {
        portaTopdownTexture = LoadTexture("assets/images/sala entrada/porta topdown.png");
        portasCarregadas = true;
    }

    if (!espelhosCarregado) {
        espelhosTexture = LoadTexture("assets/images/banheiros/espelhos.png");
        espelhosCarregado = true;
    }

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
        DrawTexturePro(portaTopdownTexture, (Rectangle){305,190,51,101},
            cur->gatilho, (Vector2){0,0}, 0.0f, WHITE);
        cur = cur->proxima;
    }
}

void DrawGameplay(Personagem player) {
    switch (salaAtual) {
        case SALA_SAGUAO:
            ClearBackground(BLACK);
            DrawTexturePro(chaoSalaTexture,
                (Rectangle){0, 0, 1921, 1081},
                (Rectangle){0, 0, 1920, 1080},
                (Vector2){0, 0}, 0.0f, WHITE);
            DesenharPortasDaSala(salaAtual);
            DrawTexturePro(bilheteriaTexture,
                (Rectangle){294, 279, 172, 197},
                bilheteria, (Vector2){0,0}, 0.0f, WHITE);
            DrawTextF("SAIDA",     910,  950, 30, DARKGRAY);
            DrawTextF("COR 1",      20,   90, 20, WHITE);
            DrawTextF("COR 2",    1750,   90, 20, WHITE);
            DrawTextF("WC F",       12,  535, 20, WHITE);
            DrawTextF("WC M",     1826,  535, 20, WHITE);
            DrawTextF("ZELADOR",  1730,  940, 18, WHITE);
            break;
        case SALA_CORREDOR_1:
            ClearBackground(BLACK);
            {
                Rectangle src = {254, 617, 225, 143};
                for (int ty = 340; ty < 740; ty += 80)
                    for (int tx = 0; tx < 1920; tx += 160)
                        DrawTextureRec(chaoTexture, src, (Vector2){(float)tx, (float)ty}, WHITE);
            }
            DrawRectangleRec(corredorParedeTopo, DARKGRAY);
            DrawRectangleRec(corredorParedeBaixo, DARKGRAY);
            DrawTexturePro(cartazesTexture, (Rectangle){0,0,1921,1081},
                (Rectangle){160,20,1600,315}, (Vector2){0,0}, 0.0f, WHITE);
            DesenharPortasDaSala(salaAtual);
            DrawTextF("CORREDOR 1", 850, 130, 40, MAROON);
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
            DrawTextF("SAIDA", 1830, 940, 22, WHITE);
            break;
        case SALA_CORREDOR_2:
            ClearBackground(BLACK);
            {
                Rectangle src = {254, 617, 225, 143};
                for (int ty = 340; ty < 740; ty += 80)
                    for (int tx = 0; tx < 1920; tx += 160)
                        DrawTextureRec(chaoTexture, src, (Vector2){(float)tx, (float)ty}, WHITE);
            }
            DrawRectangleRec(corredorParedeTopo, DARKGRAY);
            DrawRectangleRec(corredorParedeBaixo, DARKGRAY);
            DrawTexturePro(cartazesTexture, (Rectangle){0,0,1921,1081},
                (Rectangle){160,20,1600,315}, (Vector2){0,0}, 0.0f, WHITE);
            DesenharPortasDaSala(salaAtual);
            DrawTextF("CORREDOR 2", 850, 130, 40, DARKBLUE);
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
            DrawTextF("SAIDA", 10, 940, 22, WHITE);
            break;
        case SALA_BANHEIRO_FEM:
            ClearBackground(WHITE);
            {
                Rectangle src = {317, 406, 188, 126};
                for (int ty = 200; ty < 1080; ty += 126)
                    for (int tx = 0; tx < 1920; tx += 188)
                        DrawTextureRec(chaoBanheiroTexture, src, (Vector2){(float)tx, (float)ty}, WHITE);
            }
            DrawTexturePro(espelhosTexture, (Rectangle){0,0,1921,1081},
                (Rectangle){0,0,1920,260}, (Vector2){0,0}, 0.0f, WHITE);
            DrawRectangleRec(cabinesBanheiroFem, (Color){ 180, 140, 180, 255 });
            DrawRectangleRec(piasBanheiroFem, LIGHTGRAY);
            DesenharPortasDaSala(salaAtual);
            DrawTextF("BANHEIRO FEMININO", 700, 50, 40, WHITE);
            DrawTextF("CABINES", 55, 400, 20, WHITE);
            DrawTextF("PIAS", 1745, 160, 20, DARKGRAY);
            DrawTextF("SAIDA", 1830, 530, 22, WHITE);
            break;
        case SALA_BANHEIRO_MASC:
            ClearBackground(WHITE);
            {
                Rectangle src = {317, 406, 188, 126};
                for (int ty = 200; ty < 1080; ty += 126)
                    for (int tx = 0; tx < 1920; tx += 188)
                        DrawTextureRec(chaoBanheiroTexture, src, (Vector2){(float)tx, (float)ty}, WHITE);
            }
            DrawTexturePro(espelhosTexture, (Rectangle){0,0,1921,1081},
                (Rectangle){0,0,1920,260}, (Vector2){0,0}, 0.0f, WHITE);
            DrawRectangleRec(cabinesBanheiroMasc, (Color){ 140, 170, 200, 255 });
            DrawRectangleRec(piasBanheiroMasc, LIGHTGRAY);
            DesenharPortasDaSala(salaAtual);
            DrawTextF("BANHEIRO MASCULINO", 680, 50, 40, WHITE);
            DrawTextF("CABINES", 1730, 400, 20, WHITE);
            DrawTextF("PIAS", 80, 160, 20, DARKGRAY);
            DrawTextF("SAIDA", 10, 530, 22, WHITE);
            break;
        case SALA_ZELADOR:
            ClearBackground((Color){ 120, 100, 80, 255 });
            DrawTexturePro(armarioZeladorTexture, (Rectangle){3,0,94,95},
                armarioZelador, (Vector2){0,0}, 0.0f, WHITE);
            DrawTexturePro(caixaTexture, (Rectangle){9,39,97,61},
                caixasZelador, (Vector2){0,0}, 0.0f, WHITE);
            DrawTexturePro(lixeiroTexture, (Rectangle){25,14,26,28},
                depositoLixo, (Vector2){0,0}, 0.0f, WHITE);
            DesenharPortasDaSala(salaAtual);
            DrawTextF("SALA DO ZELADOR", 730, 50, 40, BEIGE);
            DrawTextF("SAIDA", 910, 950, 30, DARKGRAY);
            break;
        default:
            ClearBackground(BLACK);
            DesenharPortasDaSala(salaAtual);
            break;
    }

    #define IS_CHAVE(t) ((t)==ITEM_CHAVE||(t)==ITEM_CHAVE_SALA1||(t)==ITEM_CHAVE_SALA2||(t)==ITEM_CHAVE_BANHEIRO_FEM||(t)==ITEM_CHAVE_BANHEIRO_MASC)
    for (int i = 0; i < MAX_ITENS_MUNDO; i++) {
        if (itensMundo[i].coletado || itensMundo[i].mapa != salaAtual) continue;
        Vector2 pos = itensMundo[i].posicao;
        if (itensMundo[i].tipo == ITEM_SACO_LIXO) {
            DrawTexturePro(sacoLixoTexture, (Rectangle){0,0,80,48},
                (Rectangle){pos.x - 40, pos.y - 24, 80, 48}, (Vector2){0,0}, 0.0f, WHITE);
        } else if (IS_CHAVE(itensMundo[i].tipo)) {
            DrawTexturePro(chaveTexture, (Rectangle){757,512,112,234},
                (Rectangle){pos.x - 16, pos.y - 34, 32, 68}, (Vector2){0,0}, 0.0f, WHITE);
        } else {
            DrawCircle((int)pos.x, (int)pos.y, 14.0f, BLACK);
            DrawCircle((int)pos.x, (int)pos.y, 12.0f, CorItem(itensMundo[i].tipo));
        }
        DrawTextF(NomeItem(itensMundo[i].tipo), (int)pos.x - 20, (int)pos.y - 30, 16, WHITE);
        float dx = player.posicao.x - pos.x;
        float dy = player.posicao.y - pos.y;
        if ((dx * dx + dy * dy) <= (itensMundo[i].raioColeta * itensMundo[i].raioColeta) &&
            !InventarioEstaCheio(&player.inventario)) {
            DrawTextF(TextFormat("[E] Pegar %s", NomeItem(itensMundo[i].tipo)),
                     (int)pos.x - 80, (int)pos.y - 50, 18, YELLOW);
        }
    }

    ItemChao *curItem = itensChao.cabeca;
    while (curItem != NULL) {
        if (curItem->mapaId == (int)salaAtual) {
            if (curItem->tipo == ITEM_SACO_LIXO) {
                DrawTexturePro(sacoLixoTexture, (Rectangle){0,0,80,48},
                    (Rectangle){curItem->posicao.x - 40, curItem->posicao.y - 24, 80, 48},
                    (Vector2){0,0}, 0.0f, WHITE);
            } else if (IS_CHAVE(curItem->tipo)) {
                DrawTexturePro(chaveTexture, (Rectangle){757,512,112,234},
                    (Rectangle){curItem->posicao.x - 28, curItem->posicao.y - 58, 56, 116},
                    (Vector2){0,0}, 0.0f, WHITE);
            } else {
                DrawCircle((int)curItem->posicao.x, (int)curItem->posicao.y, 14.0f, BLACK);
                DrawCircle((int)curItem->posicao.x, (int)curItem->posicao.y, 12.0f, CorItem(curItem->tipo));
            }
            DrawTextF(NomeItem(curItem->tipo), (int)curItem->posicao.x - 20, (int)curItem->posicao.y - 30, 16, WHITE);
            float dx = player.posicao.x - curItem->posicao.x;
            float dy = player.posicao.y - curItem->posicao.y;
            if ((dx * dx + dy * dy) <= (50.0f * 50.0f)) {
                const char *prompt = InventarioEstaCheio(&player.inventario)
                    ? TextFormat("[E] Trocar por %s", NomeItem(curItem->tipo))
                    : TextFormat("[E] Pegar %s",      NomeItem(curItem->tipo));
                DrawTextF(prompt, (int)curItem->posicao.x - 90, (int)curItem->posicao.y - 50, 18, YELLOW);
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
                    Texture2D tex = (curE->variante == 0) ? pipocaTexture : pepiBlackTexture;
                    DrawTexturePro(tex, (Rectangle){0,0,80,48},
                        (Rectangle){pos.x - 70, pos.y - 42, 140, 84}, (Vector2){0,0}, 0.0f, WHITE);
                } else {
                    DrawTexturePro(sujeiraTexture, (Rectangle){0,0,80,48},
                        (Rectangle){pos.x - 70, pos.y - 42, 140, 84}, (Vector2){0,0}, 0.0f, WHITE);
                }
                float dx = player.posicao.x - pos.x;
                float dy = player.posicao.y - pos.y;
                if ((dx * dx + dy * dy) <= (80.0f * 80.0f)) {
                    if (curE->tipo == ELEMENTO_SUJEIRA && itemAtivo == ITEM_VASSOURA)
                        DrawTextF("[CLIQUE] Limpar Sujeira", (int)pos.x - 90, (int)pos.y - 35, 16, YELLOW);
                    else if (curE->tipo == ELEMENTO_LIXO && itemAtivo == ITEM_SACO_LIXO) {
                        if (lixosNoSaco < MAX_LIXOS_SACO)
                            DrawTextF("[CLIQUE] Coletar Lixo", (int)pos.x - 80, (int)pos.y - 35, 16, YELLOW);
                        else
                            DrawTextF("Saco cheio!", (int)pos.x - 45, (int)pos.y - 35, 16, RED);
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
        DrawTextF(turno->rotulo, 20, 60, 28, SKYBLUE);
    }

    if (player.inventario.ativo->item == ITEM_SACO_LIXO)
        DrawTextF(TextFormat("Lixo: %d/%d", lixosNoSaco, MAX_LIXOS_SACO),
                 20, 20, 22, lixosNoSaco >= MAX_LIXOS_SACO ? RED : WHITE);

    if (salaAtual == SALA_ZELADOR &&
        player.inventario.ativo->item == ITEM_SACO_LIXO &&
        CheckCollisionCircleRec(player.posicao, 80.0f, depositoLixo)) {
        if (lixosNoSaco >= MAX_LIXOS_SACO)
            DrawTextF("[CLIQUE] Deixar saco aqui", 1410, 415, 22, YELLOW);
        else
            DrawTextF("Saco precisa estar cheio", 1420, 415, 22, RED);
    }

    if (portaChavePrompt || portaChaveMsgTimer > 0) {
        DrawRectangle(560, 490, 800, 100, Fade(BLACK, 0.75f));
        DrawTextF("[CLIQUE] Usar chave para abrir", 600, 525, 28, YELLOW);
    }

    if (portaBloqueadaMsgTimer > 0) {
        DrawRectangle(560, 490, 800, 100, Fade(BLACK, 0.75f));
        DrawTextF("Trancado. Voce precisa de uma chave.", 580, 525, 28, RED);
    }

    if (saidaBloqueadaMsgTimer > 0) {
        DrawRectangle(440, 490, 1040, 100, Fade(BLACK, 0.75f));
        DrawTextF("Termine a limpeza antes de sair!", 510, 525, 32, RED);
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
    DrawTextF(TextFormat("Sujeira: %d  |  Lixo: %d", sujeirasRestantes, lixosRestantes),
             20, 95, 22, LIGHTGRAY);
}
