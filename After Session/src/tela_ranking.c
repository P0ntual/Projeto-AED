#include "tela_ranking.h"
#include "ranking.h"
#include "raylib.h"

static Ranking ranking;

void RecarregarRanking(void) {
    CarregarRanking(&ranking);
}

TelaAtual UpdateTelaRanking(void) {
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
        return TELA_START;
    }
    return TELA_RANKING;
}

void DrawTelaRanking(void) {
    ClearBackground(BLACK);

    DrawText("MELHORES TEMPOS", 600, 80, 70, GOLD);

    if (ranking.quantidade == 0) {
        DrawText("Nenhum tempo registrado ainda.", 600, 450, 36, GRAY);
    } else {
        for (int i = 0; i < ranking.quantidade; i++) {
            int y = 240 + i * 60;
            Color cor = (i == 0) ? GOLD : (i == 1) ? LIGHTGRAY : (i == 2) ? BROWN : WHITE;
            DrawText(TextFormat("%2d.", i + 1), 550, y, 36, cor);
            DrawText(ranking.entradas[i].nome, 650, y, 36, cor);
            DrawText(TextFormat("%6.1fs", ranking.entradas[i].tempo),
                     1100, y, 36, cor);
        }
    }

    DrawText("[ENTER] Voltar ao menu", 700, 980, 30, LIGHTGRAY);
}
