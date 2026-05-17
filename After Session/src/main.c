#include "personagem.h"
#include "raylib.h" 

//criar a janela do raylib
int main(){
    InitWindow(1920, 1080, "After Session");
    SetTargetFPS(60); 
    //cria a variavel jogador e inicializa ela
    Personagem player;
    InicializarPersonagem(&player);

    //loop
    while (!WindowShouldClose()){
      AtualizarPersonagem(&player);
       
      BeginDrawing();
        ClearBackground(SKYBLUE);
        DrawPersonagem(player);
      EndDrawing();
    }
    CloseWindow();    
    return 0;
}