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

    Rectangle btnVoltarMenu = { 760.0f, 700.0f, 400.0f, 80.0f };

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
                if (CheckCollisionPointRec(GetMousePosition(), btnVoltarMenu) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    tela = TELA_START;
                }
                break;

            case TELA_GAME_OVER:
                if (CheckCollisionPointRec(GetMousePosition(), btnVoltarMenu) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    tela = TELA_START;
                }
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
                TocarMusicaVitoria();
            }
            if (tela == TELA_RANKING) {
                RecarregarRanking();
            }
            if (tela == TELA_START) {
                ResetarGameplay(&player);
                TocarMusicaMenu();
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

                case TELA_VITORIA: {
                    int sz = 50;
                    int w = MeasureText("PARABENS! TURNO CONCLUIDO", sz);
                    DrawText("PARABENS! TURNO CONCLUIDO", (1920 - w)/2, 500, sz, GREEN);

                    bool hover = CheckCollisionPointRec(GetMousePosition(), btnVoltarMenu);
                    DrawRectangleRec(btnVoltarMenu, hover ? GREEN : DARKGREEN);
                    int btSz = 30;
                    int btW  = MeasureText("VOLTAR AO MENU", btSz);
                    DrawText("VOLTAR AO MENU",
                             (int)(btnVoltarMenu.x + btnVoltarMenu.width/2  - btW/2),
                             (int)(btnVoltarMenu.y + btnVoltarMenu.height/2 - btSz/2),
                             btSz, BLACK);
                    break;
                }

                case TELA_GAME_OVER: {
                    int sz = 60;
                    int w = MeasureText("GAME OVER", sz);
                    DrawText("GAME OVER", (1920 - w)/2, 500, sz, RED);

                    bool hover = CheckCollisionPointRec(GetMousePosition(), btnVoltarMenu);
                    DrawRectangleRec(btnVoltarMenu, hover ? RED : MAROON);
                    int btSz = 30;
                    int btW  = MeasureText("VOLTAR AO MENU", btSz);
                    DrawText("VOLTAR AO MENU",
                             (int)(btnVoltarMenu.x + btnVoltarMenu.width/2  - btW/2),
                             (int)(btnVoltarMenu.y + btnVoltarMenu.height/2 - btSz/2),
                             btSz, BLACK);
                    break;
                }

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