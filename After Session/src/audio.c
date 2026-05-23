#include "audio.h"
#include "raylib.h"

static Music musicaAtual;
static bool audioIniciado = false;

void InicializarAudio(void) {
    InitAudioDevice();
    audioIniciado = true;
}

static void pararMusicaAtual(void) {
    if (IsMusicValid(musicaAtual)) {
        StopMusicStream(musicaAtual);
        UnloadMusicStream(musicaAtual);
    }
}

void TocarMusicaMenu(void) {
    if (!audioIniciado) return;
    pararMusicaAtual();
    musicaAtual = LoadMusicStream("My_Mirror_Image.ogg");
    SetMusicVolume(musicaAtual, 0.6f);
    PlayMusicStream(musicaAtual);
}

void TocarMusicaIntro(void) {
    if (!audioIniciado) return;
    pararMusicaAtual();
    musicaAtual = LoadMusicStream("Haunting.ogg");
    SetMusicVolume(musicaAtual, 0.6f);
    PlayMusicStream(musicaAtual);
}

void AtualizarAudio(void) {
    if (!audioIniciado) return;
    if (IsMusicValid(musicaAtual)) UpdateMusicStream(musicaAtual);
}

void EncerrarAudio(void) {
    if (!audioIniciado) return;
    pararMusicaAtual();
    CloseAudioDevice();
    audioIniciado = false;
}
