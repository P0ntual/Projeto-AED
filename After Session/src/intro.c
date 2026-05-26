#include "intro.h"
#include "raylib.h"

static Texture2D telaIntro;
static bool iniciandoTransicao = false;
static float fadeAlpha = 0.0f;

void InitIntro(void) {
    telaIntro = LoadTexture("tela intro.png");
}

void UnloadIntro(void) {
    UnloadTexture(telaIntro);
}


static bool lendoDialogo = true;
static bool posicionouNaPonte = false;

TelaAtual UpdateIntro(Personagem *player) {
    if (!posicionouNaPonte) {
        player->posicao.x = 5.0f;
        player->posicao.y = 970.0f;
        posicionouNaPonte = true;
    }

    if (lendoDialogo) {
        if (IsKeyPressed(KEY_ENTER)) {
            lendoDialogo = false;
        }
        return TELA_INTRO;
    }
    if (iniciandoTransicao) {
        fadeAlpha += 0.01f;

        if (fadeAlpha >= 1.0f) {
            fadeAlpha = 0.0f;
            iniciandoTransicao = false;

            lendoDialogo = true;
            posicionouNaPonte = false;

            player->posicao.x = 960.0f;
            player->posicao.y = 1050.0f;

            return TELA_ENTRADA;
        }
        return TELA_INTRO;
    }

  
    float velocidade = 5.0f; 

 
    if (IsKeyDown(KEY_D)) {
        player->posicao.x += velocidade;
    }
    

    if (IsKeyDown(KEY_A)) {
        player->posicao.x -= velocidade;
    }



    if (player->posicao.x < 5.0f) {
        player->posicao.x = 5.0f;
    }


    if (player->posicao.x >= 1900.0f) {
        iniciandoTransicao = true;
    }

    return TELA_INTRO;
}

void DrawIntro(Personagem player) {
    ClearBackground(BLACK);

    DrawTexturePro(telaIntro,
        (Rectangle){ 0, 0, (float)telaIntro.width, (float)telaIntro.height },
        (Rectangle){ 0, 0, 1920, 1080 },
        (Vector2){ 0, 0 }, 0.0f, WHITE);


    DrawPersonagem(player);


    if (lendoDialogo) {
        Rectangle caixaDialogo = { 100.0f, 750.0f, 1720.0f, 200.0f };
        DrawRectangleRec(caixaDialogo, DARKGRAY);
        DrawRectangleLinesEx(caixaDialogo, 5.0f, BLACK); // Borda preta
        
        DrawText("Voce:", 130, 780, 30, SKYBLUE);
        DrawText("\"Finalmente consegui um emprego... acho que dessa vez vai!\"", 130, 850, 40, WHITE);
        DrawText("[PRESSIONE ENTER PARA CONTINUAR]", 1350, 910, 20, LIGHTGRAY);
    }


    if (iniciandoTransicao) {
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, fadeAlpha));
    }
}