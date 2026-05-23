# After Session

Jogo de sobrevivência em perspectiva top-down desenvolvido em C com [raylib](https://www.raylib.com/). Você é um zelador contratado para limpar o **Cinema São Luiz** durante a madrugada — mas a noite vai ser mais longa do que parecia.

> **Estado atual:** em desenvolvimento. A estrutura de telas, salas, movimentação e sistema de itens estão implementados. Inimigos (fantasmas) e a lógica de vitória/derrota ainda estão sendo desenvolvidos.

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

O executável gerado é `./jogo` e deve ser rodado a partir da raiz do projeto (onde ficam os arquivos `.ogg`).

---

## Controles

| Tecla | Ação |
|---|---|
| `W` `A` `S` `D` | Mover o personagem |
| `E` | Coletar item próximo / trocar com item ativo (se inventário cheio) |
| `Q` | Largar item ativo no chão |
| `1` `2` `3` | Selecionar slot do inventário |
| `Enter` | Avançar diálogos |
| `Clique` no botão PLAY | Iniciar o jogo |

---

## Telas e fluxo do jogo

```
TELA_START → TELA_INTRO → TELA_ENTRADA → TELA_GAMEPLAY → TELA_VITORIA
                                                        → TELA_GAME_OVER
```

| Tela | Descrição |
|---|---|
| **Start** | Tela inicial com botão PLAY e fade de transição |
| **Intro** | Diálogo de abertura; o personagem caminha até a entrada do cinema |
| **Entrada** | Fachada do Cinema São Luiz; entrar pelas portas inicia o turno |
| **Gameplay** | Jogo principal — exploração do cinema com salas, corredores e itens |
| **Vitória / Game Over** | Telas de fim de jogo (em construção) |

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
│   ├── main.c          # Loop principal e máquina de estados de telas
│   ├── audio.c         # Sistema de música (raylib Music streams)
│   ├── personagem.c    # Movimentação e renderização do jogador
│   ├── item.c          # Inventário (lista duplamente encadeada) e itens no chão
│   ├── gameplay.c      # Lógica do jogo, mapa e transições de sala
│   ├── entrada.c       # Tela da fachada do cinema
│   ├── intro.c         # Tela de introdução com diálogo
│   ├── start_screen.c  # Tela inicial
│   └── end_screen.c    # Telas de vitória e game over
├── includes/           # Headers
├── My_Mirror_Image.ogg # Música do menu
├── Haunting.ogg        # Música do jogo
└── Makefile
```

---

## Áudio

- `My_Mirror_Image.ogg` — toca na tela inicial
- `Haunting.ogg` — toca a partir da introdução até o fim do jogo
