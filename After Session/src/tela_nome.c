#include "tela_nome.h"
#include "ranking.h"
#include "gameplay.h"
#include "raylib.h"
#include <string.h>

static char nomeInput[RANKING_NOME_MAX] = "";
static int  nomeLen = 0;

void InitTelaNome(void) {
    nomeInput[0] = '\0';
    nomeLen = 0;
}

TelaAtual UpdateTelaNome(void) {
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 125 && nomeLen < RANKING_NOME_MAX - 1) {
            nomeInput[nomeLen]   = (char)key;
            nomeLen++;
            nomeInput[nomeLen]   = '\0';
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && nomeLen > 0) {
        nomeLen--;
        nomeInput[nomeLen] = '\0';
    }

    if (IsKeyPressed(KEY_ENTER) && nomeLen > 0) {
        Ranking r;
        CarregarRanking(&r);
        InserirPontuacao(&r, nomeInput, GameplayTempoDecorrido());
        SalvarRanking(&r);

        nomeInput[0] = '\0';
        nomeLen = 0;
        return TELA_RANKING;
    }

    return TELA_NOME;
}

void DrawTelaNome(void) {
    ClearBackground(BLACK);

    DrawText("VOCE SOBREVIVEU!", 600, 150, 70, GREEN);
    DrawText(TextFormat("Tempo: %.1f segundos", GameplayTempoDecorrido()),
             700, 320, 40, WHITE);

    DrawText("Digite seu nome:", 700, 460, 38, WHITE);

    Rectangle caixa = { 700.0f, 540.0f, 520.0f, 70.0f };
    DrawRectangleRec(caixa, DARKGRAY);
    DrawRectangleLinesEx(caixa, 3.0f, WHITE);

    DrawText(nomeInput, (int)caixa.x + 15, (int)caixa.y + 18, 40, WHITE);

    if (((int)(GetTime() * 2.0)) % 2 == 0) {
        int textWidth = MeasureText(nomeInput, 40);
        DrawRectangle((int)caixa.x + 15 + textWidth + 3,
                      (int)caixa.y + 18, 4, 40, WHITE);
    }

    DrawText("[ENTER] Salvar", 700, 660, 28, LIGHTGRAY);
}
