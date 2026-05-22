#include "gameplay.h"
#include "raylib.h"


typedef enum {
    MAPA_SAGUAO,
    MAPA_CORREDOR_1,
    MAPA_SALA_1,
    MAPA_CORREDOR_2,
    MAPA_SALA_2
} MapaAtual;

static MapaAtual mapaAtual = MAPA_SAGUAO; 

static Rectangle portaCorredor1 = { 0.0f, 0.0f, 200.0f, 200.0f };
static Rectangle portaCorredor2 = { 1720.0f, 0.0f, 200.0f, 200.0f };
static Rectangle portaSaida     = { 760.0f, 900.0f, 400.0f, 160.0f };

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


TelaAtual UpdateGameplay(Personagem *player) {
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
    }


    DrawPersonagem(player);
}