#include "raylib.h" 

//criar a janela do raylib
int main(){
    InitWindow(1920, 1080, "After Session");
    SetTargetFPS(60); 

    //posicao e tamanho do circulo
    Vector2 posicao = { 960, 540 };
    float raio = 20.0f;
    float velocidade = 3.5f;

    //loop para desenhar o circulo na tela
    while (!WindowShouldClose()){
        //movimentacao do circulo
        if (IsKeyDown(KEY_W)) posicao.y -= velocidade;
        if (IsKeyDown(KEY_A)) posicao.x -= velocidade;   
        if (IsKeyDown(KEY_S)) posicao.y += velocidade;
        if (IsKeyDown(KEY_D)) posicao.x += velocidade;

    // desenho do circulo
    BeginDrawing();
        ClearBackground(SKYBLUE);
        DrawCircle((int)posicao.x, (int)posicao.y, raio, BLUE);
    EndDrawing();
}
    //fechamento da janela 
    CloseWindow();    
    return 0;
}