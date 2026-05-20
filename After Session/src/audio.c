#include "audio.h"
#include "raylib.h"

static Music musicaAtual;
static bool audioIniciado = false;

void InicializarAudio(void) {
    InitAudioDevice();
    audioIniciado = true;
}

void TocarMusicaMenu(void) {
    if (!audioIniciado) return;

    if (IsMusicStreamPlaying(musicaAtual)) {
        StopMusicStream(musicaAtual);
        UnloadMusicStream(musicaAtual);
    }

    musicaAtual = LoadMusicStream("My_Mirror_Image.ogg");
    SetMusicVolume(musicaAtual, 0.6f);
    PlayMusicStream(musicaAtual);
}

void TocarMusicaIntro(void) {
    if (!audioIniciado) return;

    if (IsMusicStreamPlaying(musicaAtual)) {
        StopMusicStream(musicaAtual);
        UnloadMusicStream(musicaAtual);
    }

    musicaAtual = LoadMusicStream("Haunting.ogg");
    SetMusicVolume(musicaAtual, 0.6f);
    PlayMusicStream(musicaAtual);
}

// Chame essa função no loop principal para manter o stream ativo
void AtualizarAudio(void) {
    if (!audioIniciado) return;
    UpdateMusicStream(musicaAtual);
}

void EncerrarAudio(void) {
    if (!audioIniciado) return;
    StopMusicStream(musicaAtual);
    UnloadMusicStream(musicaAtual);
    CloseAudioDevice();
}