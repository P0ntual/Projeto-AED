#include "personagem.h"
#include "telas.h"
#include "start_screen.h"
#include "intro.h"
#include "gameplay.h"
#include "raylib.h" 

int main() {
    // Criar a janela do raylib
    InitWindow(1920, 1080, "After Session");
    SetTargetFPS(60); 
    
    TelaAtual tela = TELA_START;
    
    // Cria a variável jogador e inicializa ela
    Personagem player;
    InicializarPersonagem(&player);

    // Loop principal
    while (!WindowShouldClose()) {
        
        // ==========================================
        // 1. UPDATE / LÓGICA (Nenhum desenho aqui!)
        // ==========================================
        switch (tela) {
            case TELA_START:
                tela = UpdateStartScreen();
                // CORREÇÃO 1: Removido o DrawStartScreen() daqui. 
                // Desenhos só podem acontecer entre BeginDrawing e EndDrawing.
                break;
                
            case TELA_INTRO:
                // O personagem só atualiza a posição quando o jogo começar de verdade!
                tela = UpdateIntro(&player); 
                break;
                
            case TELA_GAMEPLAY:
                AtualizarPersonagem(&player);
                tela = UpdateGameplay(&player); 
                break;
                
            case TELA_VITORIA:
                break;
                
            case TELA_GAME_OVER: // CORREÇÃO 2: Removido o "_" para bater exatamente com o seu enum (TELA_GAMEOVER)
                break;  
                
            default: break;
        }
        
        // ==========================================
        // 2. DRAW / DESENHO (Tudo que vai para a tela fica aqui)
        // ==========================================
        BeginDrawing();
            ClearBackground(BLACK);

            switch (tela) {
                case TELA_START:
                    DrawStartScreen(); // O menu e o fade preto são desenhados aqui
                    break;
                    
                case TELA_INTRO:
                    // CORREÇÃO 3: O personagem agora foi movido para DENTRO dos cases certos!
                    DrawIntro(player); // O cenário e o personagem são desenhados aqui 
                    break;
                    
                case TELA_GAMEPLAY:
                    DrawGameplay(player); 
                    break;
                    
                case TELA_VITORIA:
                    DrawText("PARABENS! TURNO CONCLUIDO", 500, 500, 50, GREEN);
                    break;
                    
                case TELA_GAME_OVER:
                    DrawText("GAME OVER", 800, 500, 60, RED);
                    break;
                    
                default: break;
            }

            // ATENÇÃO: O DrawPersonagem(player) saiu daqui de baixo!
            
        EndDrawing();
    }
    
    CloseWindow();    
    return 0;
}