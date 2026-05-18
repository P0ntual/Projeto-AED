#include "personagem.h"
#include "telas.h"
#include "start_screen.h"
#include "raylib.h" 

//criar a janela do raylib
int main(){
    InitWindow(1920, 1080, "After Session");
    SetTargetFPS(60); 
    TelaAtual tela = TELA_START;
    //cria a variavel jogador e inicializa ela
    Personagem player;
    InicializarPersonagem(&player);

    //loop
    while (!WindowShouldClose()){
      switch (tela) {
        case TELA_START:
          tela = UpdateStartScreen();
          DrawStartScreen();
          break;
        case TELA_INTRO:
          //tela de introdução
          break;
        case TELA_GAMEPLAY:
          //tela de gameplay
          break;
        case TELA_VITORIA:
          //tela de vitória
          break;
        case TELA_GAME_OVER:
          //tela de game over
          break;  
          default: break;
      }
      AtualizarPersonagem(&player);
       
      BeginDrawing();
        ClearBackground(SKYBLUE);
        DrawPersonagem(player);
      EndDrawing();
    }
    CloseWindow();    
    return 0;
}