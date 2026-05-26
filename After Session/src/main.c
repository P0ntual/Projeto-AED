#include "personagem.h"
#include "telas.h"
#include "start_screen.h"
#include "intro.h"
#include "entrada.h"
#include "gameplay.h"
#include "tela_nome.h"
#include "tela_ranking.h"
#include "audio.h"
#include "raylib.h"

int main() {
    InitWindow(1920, 1080, "After Session");
    SetTargetFPS(60);

    InicializarAudio();
    InitStartScreen();
    InitIntro();

    TelaAtual tela = TELA_START;
    TelaAtual telaAnterior = TELA_START;

    Personagem player;
    InicializarPersonagem(&player);


    TocarMusicaMenu();

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_F11)) ToggleFullscreen();

        switch (tela) {
            case TELA_START:
                tela = UpdateStartScreen();
                break;

            case TELA_INTRO:
                tela = UpdateIntro(&player);
                break;

            case TELA_ENTRADA:
                AtualizarPersonagem(&player);
                tela = UpdateEntrada(&player);
                break;

            case TELA_GAMEPLAY:
                AtualizarPersonagem(&player);
                tela = UpdateGameplay(&player);
                break;

            case TELA_VITORIA:
                break;

            case TELA_GAME_OVER:
                break;

            case TELA_NOME:
                tela = UpdateTelaNome();
                break;

            case TELA_RANKING:
                tela = UpdateTelaRanking();
                break;

            default: break;
        }

        if (tela != telaAnterior) {
            if (tela == TELA_INTRO) {
                TocarMusicaIntro();
            }
            if (tela == TELA_NOME) {
                InitTelaNome();
            }
            if (tela == TELA_RANKING) {
                RecarregarRanking();
            }
            telaAnterior = tela;
        }


        AtualizarAudio();


        BeginDrawing();
            ClearBackground(BLACK);
            AtualizarAudio();
            switch (tela) {
                case TELA_START:
                    DrawStartScreen();
                    break;

                case TELA_INTRO:
                    DrawIntro(player);
                    break;

                case TELA_ENTRADA:
                    DrawEntrada(player);
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

                case TELA_NOME:
                    DrawTelaNome();
                    break;

                case TELA_RANKING:
                    DrawTelaRanking();
                    break;

                default: break;
            }
            AtualizarAudio();
        EndDrawing();
        AtualizarAudio();
    }

    UnloadStartScreen();
    UnloadIntro();
    EncerrarAudio();
    CloseWindow();
    return 0;
}