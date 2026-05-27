#include "audio.h"
#include "raylib.h"

static Music musicaAtual;
static bool audioIniciado = false;

void InicializarAudio(void) {
    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(4096);
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
    musicaAtual = LoadMusicStream("My_Mirror_Image.wav");
    SetMusicVolume(musicaAtual, 0.6f);
    SetMusicPitch(musicaAtual, 1.0f);
    musicaAtual.looping = true;
    PlayMusicStream(musicaAtual);
}

void TocarMusicaIntro(void) {
    if (!audioIniciado) return;
    pararMusicaAtual();
    musicaAtual = LoadMusicStream("Haunting.wav");
    SetMusicVolume(musicaAtual, 0.6f);
    SetMusicPitch(musicaAtual, 1.0f);
    musicaAtual.looping = true;
    PlayMusicStream(musicaAtual);
}

void TocarMusicaGame(void) {
    if (!audioIniciado) return;
    pararMusicaAtual();
    musicaAtual = LoadMusicStream("Haunting.wav");
    SetMusicVolume(musicaAtual, 0.6f);
    SetMusicPitch(musicaAtual, 1.0f);
    musicaAtual.looping = true;
    PlayMusicStream(musicaAtual);
}

void TocarMusicaPerseguicao(void) {
    if (!audioIniciado) return;
    pararMusicaAtual();
    musicaAtual = LoadMusicStream("Ghost_Appear.wav");
    SetMusicVolume(musicaAtual, 0.8f);
    SetMusicPitch(musicaAtual, 1.0f);
    musicaAtual.looping = true;
    PlayMusicStream(musicaAtual);
}

void AtualizarAudio(void) {
    if (!audioIniciado) return;
    UpdateMusicStream(musicaAtual);
}

void EncerrarAudio(void) {
    if (!audioIniciado) return;
    pararMusicaAtual();
    CloseAudioDevice();
    audioIniciado = false;
}