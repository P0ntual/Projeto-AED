#include "start_screen.h"
#include "raylib.h"

// Variáveis do botão
static Rectangle btnPlay = { 960.0f/2.0f - 250.0f, 600.0f, 500.0f, 80.0f };
static bool mouseNoBotao = false;

// Variáveis da transição
static bool iniciandoTransicao = false;
static float fadeAlpha = 0.0f;

TelaAtual UpdateStartScreen(void) {
    // 1. Lógica do Fade (Acontece DEPOIS que o botão foi clicado)
    if (iniciandoTransicao) {
        fadeAlpha += 0.008f; // Velocidade do fade
        
        // Quando a tela ficar 100% preta
        if (fadeAlpha >= 1.0f) {
            fadeAlpha = 0.0f;           // Zera o alpha para a próxima vez que voltar ao menu
            iniciandoTransicao = false; // Desliga a transição
            return TELA_INTRO;          // Transição completa, vá para a próxima tela!
        }
        
        return TELA_START; // Continua na tela Start enquanto escurece
    }
    
    // 2. Lógica do Botão (Enquanto a transição não começou)
    Vector2 mousePoint = GetMousePosition();
    
    if (CheckCollisionPointRec(mousePoint, btnPlay)) {
        mouseNoBotao = true;
        
        // O GATILHO DA TRANSIÇÃO
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            iniciandoTransicao = true; 
        }
    } else {
        mouseNoBotao = false;
    }
    
    return TELA_START;
}

void DrawStartScreen(void) {
    ClearBackground(BLACK);
    
    // Cor do botão com efeito Hover
    Color corDoBotao;
    if (mouseNoBotao) {
        corDoBotao = GREEN;
    } else {
        corDoBotao = DARKGREEN;
    }
    
    // Desenha o botão
    DrawRectangleRec(btnPlay, corDoBotao);
    
    // Desenha o texto centralizado no botão
    int tamanhoTexto = 40;
    int larguraTexto = MeasureText("PLAY", tamanhoTexto);
    DrawText("PLAY", 
             (int)(btnPlay.x + btnPlay.width/2 - larguraTexto/2), 
             (int)(btnPlay.y + btnPlay.height/2 - tamanhoTexto/2),
             tamanhoTexto, 
             BLACK);
             
    // ==========================================
    // EFEITO DE TRANSIÇÃO (SEMPRE POR ÚLTIMO)
    // ==========================================
    if (iniciandoTransicao) {
        // Desenha um retângulo preto gigante por cima de tudo com a transparência (fadeAlpha) subindo
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, fadeAlpha));
    }
}