# After Session

Jogo de sobrevivência em perspectiva top-down desenvolvido em C com [raylib](https://www.raylib.com/). Você é um zelador contratado para limpar o **Cinema São Luiz** durante a madrugada — mas a noite vai ser mais longa do que parecia.

---

## Dependências

| Dependência | Versão testada |
|---|---|
| GCC | qualquer recente |
| GNU Make | qualquer |
| raylib | 6.x |

### Instalando o raylib (Ubuntu/Debian/WSL)

```bash
sudo apt update
sudo apt install libraylib-dev
# ou compile da fonte: https://github.com/raysan5/raylib
```

No **Windows**, o projeto é compilado via **WSL2** (Windows Subsystem for Linux).

---

## Compilar e executar

```bash
# Compilar
make

# Compilar e executar direto
make run

# Remover o executável
make clean
```

O executável gerado é `./jogo` e deve ser rodado a partir da raiz do projeto.

---

## Controles

| Tecla | Ação |
|---|---|
| `W` `A` `S` `D` | Mover o personagem |
| `E` | Coletar item próximo / trocar com item ativo (se inventário cheio) |
| `Q` | Largar item ativo no chão |
| `1` `2` `3` | Selecionar slot do inventário |
| `Enter` | Avançar diálogos |
| `F11` | Alternar tela cheia |
| `Clique` no botão PLAY | Iniciar o jogo |
| `Clique` no botão RANKING | Ver ranking na tela inicial |

---

## Telas e fluxo do jogo

```
TELA_START ──────────────────────────────────────────► TELA_RANKING
    │
    ▼
TELA_INTRO
    │
    ▼
TELA_ENTRADA
    │
    ▼
TELA_GAMEPLAY ──► TELA_VITORIA ──► TELA_NOME ──► TELA_RANKING ──► TELA_START
              └──► TELA_GAME_OVER ─┘
```

| Tela | Descrição |
|---|---|
| **Start** | Tela inicial com botão PLAY, botão RANKING e fade de transição |
| **Intro** | Diálogo de abertura; o personagem caminha até a entrada do cinema |
| **Entrada** | Fachada do Cinema São Luiz; entrar pelas portas inicia o turno |
| **Gameplay** | Jogo principal — exploração do cinema com salas, corredores, itens e inimigos |
| **Vitória** | Exibida ao concluir o turno; leva ao cadastro de nome |
| **Game Over** | Exibida ao ser eliminado; leva ao cadastro de nome |
| **Nome** | Cadastro do nome do jogador para o ranking |
| **Ranking** | Top 10 melhores tempos salvos em disco |

---

## Mapa do cinema

O gameplay se passa em 8 ambientes conectados:

```
                  [Sala do Zelador]
                        |
[Banheiro Fem] — [Saguão Principal] — [Banheiro Masc]
                   /         \
          [Corredor 1]    [Corredor 2]
               |                |
           [Sala 1]          [Sala 2]
```

- **Saguão:** hub central com acesso a todos os outros ambientes
- **Corredores 1 e 2:** ligam o saguão às salas de cinema
- **Salas 1 e 2:** salas de exibição com poltronas e tela (blocos sólidos)
- **Banheiro Feminino / Masculino:** cabines e pias como obstáculos
- **Sala do Zelador:** onde ficam os itens de limpeza

---

## Itens

| Item | Onde encontrar |
|---|---|
| Chave | Saguão |
| Vassoura | Sala do Zelador |
| Lanterna | Sala do Zelador |
| Saco de Lixo | Sala do Zelador |
| Pilha | Sala do Zelador |

O inventário tem **3 slots**. Itens podem ser largados no chão (`Q`) e trocados com o slot ativo (`E` quando inventário cheio).

---

## Estrutura do projeto

```
.
├── src/
│   ├── main.c           # Loop principal e máquina de estados de telas
│   ├── audio.c          # Sistema de música (raylib Music streams)
│   ├── personagem.c     # Movimentação e renderização do jogador
│   ├── item.c           # Inventário (lista duplamente encadeada) e itens no chão
│   ├── gameplay.c       # Lógica do jogo, mapa, inimigos e transições de sala
│   ├── inimigos.c       # Movimentação, colisão e reset dos monstros
│   ├── grafo.c          # Estrutura de grafo para o mapa de salas
│   ├── entrada.c        # Tela da fachada do cinema
│   ├── intro.c          # Tela de introdução com diálogo
│   ├── start_screen.c   # Tela inicial
│   ├── end_screen.c     # Telas de vitória e game over
│   ├── tela_nome.c      # Cadastro de nome para o ranking
│   ├── tela_ranking.c   # Exibição do ranking
│   ├── ranking.c        # Leitura e escrita do ranking em disco
│   └── expediente.c     # Lógica de turno e cronômetro
├── includes/            # Headers (.h)
├── assets/
│   ├── tela_inicial.png # Imagem da tela inicial
│   ├── tela_intro.png   # Imagem da tela de introdução
│   ├── tela_frente.png  # Fachada do cinema
│   ├── My_Mirror_Image.wav  # Música do menu
│   └── Haunting.wav     # Música do jogo
└── Makefile
```

---

## Áudio

- `assets/My_Mirror_Image.wav` — toca na tela inicial
- `assets/Haunting.wav` — toca a partir da introdução até o fim do jogo
