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

// Portas Padrão para os Corredores e Salas
static Rectangle portaEsquerda = { 0.0f, 400.0f, 100.0f, 280.0f };  
static Rectangle portaDireita  = { 1820.0f, 400.0f, 100.0f, 280.0f };


TelaAtual UpdateGameplay(Personagem *player) {
    
    float raio = 20.0f;
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
            break;


        case MAPA_CORREDOR_1:
            if (CheckCollisionCircleRec(player->posicao, raio, portaDireita)) {
                mapaAtual = MAPA_SAGUAO;
                player->posicao.x = 250.0f;
                player->posicao.y = 100.0f;
            }

            else if (CheckCollisionCircleRec(player->posicao, raio, portaEsquerda)) {
                mapaAtual = MAPA_SALA_1;
                player->posicao.x = 1750.0f;
            }
            break;

   
        case MAPA_SALA_1:

            if (CheckCollisionCircleRec(player->posicao, raio, portaDireita)) {
                mapaAtual = MAPA_CORREDOR_1;
                player->posicao.x = 150.0f;
            }
            break;

     
        case MAPA_CORREDOR_2:
       
            if (CheckCollisionCircleRec(player->posicao, raio, portaDireita)) {
                mapaAtual = MAPA_SALA_2;
                player->posicao.x = 150.0f; 
            }
            
            else if (CheckCollisionCircleRec(player->posicao, raio, portaEsquerda)) {
                mapaAtual = MAPA_SAGUAO;
                player->posicao.x = 1650.0f; 
                player->posicao.y = 100.0f;
            }
            break;

     
        case MAPA_SALA_2:
        
            if (CheckCollisionCircleRec(player->posicao, raio, portaEsquerda)) {
                mapaAtual = MAPA_CORREDOR_2;
                player->posicao.x = 1750.0f; 
            }
            break;
    }

    return TELA_GAMEPLAY; 
}

void DrawGameplay(Personagem player) {
    

    switch (mapaAtual) {
        
        case MAPA_SAGUAO:
            ClearBackground(DARKGRAY); 
            DrawText("SAGUAO PRINCIPAL", 800, 50, 40, WHITE);
            DrawRectangleRec(portaCorredor1, MAROON); 
            DrawRectangleRec(portaCorredor2, DARKBLUE); 
            break;

        case MAPA_CORREDOR_1:
            ClearBackground(BLACK); 
            DrawText("CORREDOR 1", 850, 50, 40, MAROON);
            DrawRectangleRec(portaEsquerda, GRAY); 
            DrawRectangleRec(portaDireita, MAROON); 
            break;

        case MAPA_SALA_1:
            ClearBackground(MAROON);
            DrawText("SALA 1", 900, 50, 40, WHITE);
            DrawRectangleRec(portaDireita, BLACK);
            break;

        case MAPA_CORREDOR_2:
            ClearBackground(BLACK); 
            DrawText("CORREDOR 2", 850, 50, 40, DARKBLUE);
            DrawRectangleRec(portaEsquerda, GRAY); 
            DrawRectangleRec(portaDireita, DARKBLUE); 
            break;

        case MAPA_SALA_2:
            ClearBackground(DARKBLUE); 
            DrawText("SALA 2", 900, 50, 40, WHITE);
            DrawRectangleRec(portaEsquerda, BLACK); 
            break;
    }


    DrawPersonagem(player);
}