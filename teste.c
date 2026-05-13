#include "raylib.h"

int main() {
    // Inicializa a janela (Largura, Altura, Título)
    InitWindow(800, 450, "Meu Primeiro Jogo em C");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE); // Limpa a tela com branco
            DrawText("Parabens! Raylib funcionando!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}