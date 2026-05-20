#include "gameplay.h"
#include "raylib.h"

// ==========================================
// 1. LISTA DE MAPAS DISPONÍVEIS
// ==========================================
typedef enum {
    MAPA_SAGUAO,
    MAPA_CORREDOR_1,
    MAPA_SALA_1,
    MAPA_CORREDOR_2,
    MAPA_SALA_2
} MapaAtual;

static MapaAtual mapaAtual = MAPA_SAGUAO; // O jogo sempre começa no saguão

// ==========================================
// 2. RETÂNGULOS DAS PORTAS (Gatilhos)
// ==========================================
// Portas do Saguão (Quinas superiores)
static Rectangle portaCorredor1 = { 0.0f, 0.0f, 200.0f, 200.0f };
static Rectangle portaCorredor2 = { 1720.0f, 0.0f, 200.0f, 200.0f };

// Portas Padrão para os Corredores e Salas
static Rectangle portaEsquerda = { 0.0f, 400.0f, 100.0f, 280.0f };   // Para voltar
static Rectangle portaDireita  = { 1820.0f, 400.0f, 100.0f, 280.0f }; // Para avançar


TelaAtual UpdateGameplay(Personagem *player) {
    
    float raio = 20.0f; // O raio do círculo do personagem

    // ==========================================
    // PAREDES INVISÍVEIS (Limites da Tela)
    // ==========================================
    
    // Parede da Esquerda
    if (player->posicao.x < raio) {
        player->posicao.x = raio;
    }
    // Parede da Direita (1920 é a largura da tela)
    if (player->posicao.x > 1920.0f - raio) {
        player->posicao.x = 1920.0f - raio;
    }
    // Teto (Parede de Cima)
    if (player->posicao.y < raio) {
        player->posicao.y = raio;
    }
    // Chão (Parede de Baixo - 1080 é a altura da tela)
    if (player->posicao.y > 1080.0f - raio) {
        player->posicao.y = 1080.0f - raio;
    }

    // ==========================================
    // LÓGICA DE PORTAS E MAPAS
    // ==========================================
    switch (mapaAtual) {
        // ---------------------------------------------
        case MAPA_SAGUAO:
            if (CheckCollisionCircleRec(player->posicao, raio, portaCorredor1)) {
                mapaAtual = MAPA_CORREDOR_1;
                player->posicao.x = 150.0f; 
                player->posicao.y = 540.0f; 
            }
            else if (CheckCollisionCircleRec(player->posicao, raio, portaCorredor2)) {
                mapaAtual = MAPA_CORREDOR_2;
                player->posicao.x = 150.0f; 
                player->posicao.y = 540.0f; 
            }
            break;

        // ---------------------------------------------
        case MAPA_CORREDOR_1:
            // Se encostar na porta da Direita -> Vai pra Sala 1
            if (CheckCollisionCircleRec(player->posicao, raio, portaDireita)) {
                mapaAtual = MAPA_SALA_1;
                player->posicao.x = 150.0f; 
            }
            // Se encostar na porta da Esquerda -> Volta pro Saguão
            else if (CheckCollisionCircleRec(player->posicao, raio, portaEsquerda)) {
                mapaAtual = MAPA_SAGUAO;
                player->posicao.x = 250.0f; 
                player->posicao.y = 100.0f;
            }
            break;

        // ---------------------------------------------
        case MAPA_SALA_1:
            // Voltar para o corredor 1
            if (CheckCollisionCircleRec(player->posicao, raio, portaEsquerda)) {
                mapaAtual = MAPA_CORREDOR_1;
                player->posicao.x = 1750.0f; 
            }
            break;

        // ---------------------------------------------
        case MAPA_CORREDOR_2:
            // Se encostar na porta da Direita -> Vai pra Sala 2
            if (CheckCollisionCircleRec(player->posicao, raio, portaDireita)) {
                mapaAtual = MAPA_SALA_2;
                player->posicao.x = 150.0f; 
            }
            // Se encostar na porta da Esquerda -> Volta pro Saguão
            else if (CheckCollisionCircleRec(player->posicao, raio, portaEsquerda)) {
                mapaAtual = MAPA_SAGUAO;
                player->posicao.x = 1650.0f; 
                player->posicao.y = 100.0f;
            }
            break;

        // ---------------------------------------------
        case MAPA_SALA_2:
            // Voltar para o corredor 2
            if (CheckCollisionCircleRec(player->posicao, raio, portaEsquerda)) {
                mapaAtual = MAPA_CORREDOR_2;
                player->posicao.x = 1750.0f; 
            }
            break;
    }

    return TELA_GAMEPLAY; 
}

void DrawGameplay(Personagem player) {
    
    // Desenha o mapa dependendo de onde o jogador está
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
            DrawRectangleRec(portaEsquerda, BLACK); 
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

    // Desenha o personagem por cima do mapa atual
    DrawPersonagem(player);
}