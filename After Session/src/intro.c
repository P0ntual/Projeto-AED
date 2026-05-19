#include "intro.h"
#include "raylib.h"

// Variáveis da transição
static bool iniciandoTransicao = false;
static float fadeAlpha = 0.0f;

TelaAtual UpdateIntro(Personagem *player) {
    // 1. Lógica do Fade (Se encostou na porta, perde o controle e a tela escurece)
    if (iniciandoTransicao) {
        fadeAlpha += 0.01f; 
        
        if (fadeAlpha >= 1.0f) {
            fadeAlpha = 0.0f;
            iniciandoTransicao = false;
            
            // Coloca o personagem de volta na esquerda para começar o Gameplay
            player->posicao.x = 10.0f; 
            
            return TELA_GAMEPLAY; 
        }
        return TELA_INTRO;
    }

    // 2. MOVIMENTAÇÃO MANUAL DO JOGADOR
    float velocidade = 5.0f; // Pode alterar para deixar o personagem mais rápido ou lento

    // Movimento para a Direita (Seta Direita ou tecla 'D')
    if (IsKeyDown(KEY_D)) {
        player->posicao.x += velocidade;
    }
    
    // Movimento para a Esquerda (Seta Esquerda ou tecla 'A')
    if (IsKeyDown(KEY_A)) {
        player->posicao.x -= velocidade;
    }

    // 3. LIMITES DA TELA (Colisões invisíveis)
    // Impede que o jogador saia da tela pela esquerda
    if (player->posicao.x < 5.0f) {
        player->posicao.x = 5.0f;
    }

    // Se o jogador chegar no canto direito (Porta do cinema), ativa a transição
    if (player->posicao.x >= 1900.0f) {
        iniciandoTransicao = true;
    }

    return TELA_INTRO;
}

void DrawIntro(Personagem player) {
    ClearBackground(WHITE);

    // Desenha o personagem na posição controlada pelo teclado
    DrawPersonagem(player);

    // Cortina preta da transição
    if (iniciandoTransicao) {
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, fadeAlpha));
    }
}