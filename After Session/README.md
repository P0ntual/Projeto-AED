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

O executável gerado é `./jogo` e deve ser rodado a partir da raiz do projeto (a pasta `assets/` precisa estar acessível).

---

## Controles

| Tecla / Ação | Função |
|---|---|
| `W` `A` `S` `D` | Mover o personagem |
| `E` | Coletar item próximo / trocar com item ativo (se inventário cheio) |
| `Q` | Largar item ativo no chão |
| `1` `2` `3` | Selecionar slot do inventário |
| Clique esquerdo (com vassoura) | Varrer sujeira |
| Clique esquerdo (com saco de lixo) | Coletar lixo |
| Clique esquerdo (em porta trancada) | Usar chave para destrancar |
| `Enter` | Avançar diálogo (intro) / Confirmar nome (ranking) |
| `Backspace` | Apagar caractere no nome do ranking |
| `F11` | Alternar tela cheia |
| Clique em botões | Navegar pelos menus (PLAY, RANKING, VOLTAR AO MENU) |

---

## Telas e fluxo do jogo

```
TELA_START ──────────────────────────────────────────► TELA_RANKING
    │                                                       │
    ▼                                                       ▼
TELA_INTRO                                              TELA_START
    │
    ▼
TELA_ENTRADA
    │
    ▼
TELA_GAMEPLAY ──► TELA_NOME ──► TELA_RANKING ──► TELA_START
              └─► TELA_GAME_OVER ──────────────► TELA_START
```

| Tela | Descrição |
|---|---|
| **Start** | Tela inicial com botões PLAY e RANKING |
| **Intro** | Diálogo de abertura; o personagem caminha sobre a ponte até a entrada do cinema |
| **Entrada** | Fachada do Cinema São Luiz; entrar pelas portas inicia o turno |
| **Gameplay** | Jogo principal — exploração do cinema com salas, corredores, itens e fantasmas |
| **Nome** | Cadastro do nome do jogador após a vitória |
| **Ranking** | Exibe o top 10 melhores tempos salvos em `ranking.txt` |
| **Game Over** | Exibida ao ser pego por um fantasma ou ao acabar o expediente sem ter limpado o cinema; botão para voltar ao menu |

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

- **Saguão:** hub central com bilheteria (balcão sólido) e acesso a todos os outros ambientes
- **Corredores 1 e 2:** corredores horizontais (com muros no topo e na base) que ligam o saguão às salas de cinema
- **Salas 1 e 2:** salas de exibição com telão e poltronas como blocos sólidos
- **Banheiro Feminino / Masculino:** cabines e pias como obstáculos
- **Sala do Zelador:** armário e caixas como obstáculos; onde ficam os itens de limpeza e as chaves secundárias

A conexão entre salas é representada por um **grafo (lista de adjacência)**. Cada porta é uma aresta; algumas exigem chave para serem atravessadas.

---

## Itens

| Item | Onde encontrar | Uso |
|---|---|---|
| Chave (zelador) | Bilheteria do saguão | Abre a porta da sala do zelador |
| Chave Banh. Fem | Sala do Zelador | Abre o banheiro feminino |
| Chave Banh. Masc | Sala do Zelador | Abre o banheiro masculino |
| Chave Sala 1 | Sala do Zelador | Abre a sala de cinema 1 |
| Chave Sala 2 | Sala do Zelador | Abre a sala de cinema 2 |
| Vassoura | Sala do Zelador | Varre sujeira (clique esquerdo) |
| Saco de Lixo | Sala do Zelador | Coleta até 5 lixos (clique esquerdo) |
| Lanterna | Sala do Zelador | (item temático, sem uso atual) |
| Pilha | Sala do Zelador | (item temático, sem uso atual) |

O inventário tem **3 slots** (lista duplamente encadeada com ordenação por **Insertion Sort**). Itens podem ser largados no chão (`Q`) e trocados com o slot ativo (`E` quando inventário cheio).

---

## Inimigos

Dois tipos de fantasma, controlados por uma **fila circular**:

| Tipo | Cor | Velocidade | Comportamento |
|---|---|---|---|
| **Stalker** | Branco | 50% do jogador | Sempre persegue o jogador |
| **Witch** | Roxo | 80% do jogador | Fica parado; só persegue se o jogador entrar no raio de 250px |

- As aparições só começam quando o jogador **pega a chave principal** OU quando o expediente atinge **00h** (o que vier primeiro)
- Após o gatilho, a primeira aparição é precedida por um delay aleatório de **1 a 3s**
- Cada sala tem **3 stalkers + 2 witches** (em ordem aleatória), aparecendo a cada **3s**
- Ao mudar de sala, todos os fantasmas são **resetados** e nascem novamente do zero
- Os fantasmas só nascem em **áreas caminháveis** da sala atual, a pelo menos 300px do jogador

---

## Expediente e condição de vitória

O turno do zelador vai de **22h até 05h** — 8 horas (`NoTurno`) em uma **lista encadeada**, cada uma durando 30 segundos reais. A música muda automaticamente entre música ambiente e música de perseguição conforme aparecem fantasmas.

**Vitória** (limpeza completa + uma das duas):
- Sair pela porta de saída do saguão, **ou**
- Sobreviver até as 05h

A saída fica bloqueada enquanto a limpeza não estiver completa (todas as sujeiras varridas e todos os lixos coletados do chão).

**Derrota:**
- Um fantasma encostou no jogador, **ou**
- Chegou em 05h sem ter limpado tudo

Ao vencer, o jogador digita o nome (até 12 caracteres) e o tempo é registrado no `ranking.txt` (top 10).

---

## Algoritmos e estruturas de dados

| Estrutura | Onde | Para quê |
|---|---|---|
| **Grafo** (lista de adjacência) | `grafo.c` | Conexões entre salas; portas como arestas com chave |
| **Fila circular** | `inimigos.c` | Fila de aparições dos fantasmas |
| **Lista encadeada** | `expediente.c` | Sequência de turnos (22h → 05h) |
| **Lista duplamente encadeada** | `item.c` (`Inventario`) | Inventário do jogador |
| **Lista simplesmente encadeada** | `item.c` (`ListaItensChao`) | Itens largados no chão |
| **Lista circular** | `item.c` (`ListaElementos`) | Sujeiras e lixos espalhados no mapa |
| **Insertion Sort** | `item.c` (`OrdenarInventario`) | Mantém o inventário ordenado por tipo |
| **QuickSort** | `gameplay.c` (`quickSortCandidatos`) | Ordena candidatos antes de espalhar sujeira/lixo |

---

## Estrutura do projeto

```
.
├── src/
│   ├── main.c           # Loop principal e máquina de estados de telas
│   ├── audio.c          # Sistema de música (raylib Music streams)
│   ├── personagem.c     # Movimentação e renderização do jogador
│   ├── item.c           # Inventário (Insertion Sort) e itens no chão
│   ├── gameplay.c       # Lógica do jogo, mapa, transições e gatilhos
│   ├── inimigos.c       # Fila de fantasmas, movimento e colisão
│   ├── grafo.c          # Grafo das salas
│   ├── expediente.c     # Lista encadeada de turnos
│   ├── entrada.c        # Tela da fachada do cinema
│   ├── intro.c          # Tela de introdução com diálogo
│   ├── start_screen.c   # Tela inicial
│   ├── end_screen.c     # (reservado para telas de fim de jogo)
│   ├── tela_nome.c      # Cadastro de nome para o ranking
│   ├── tela_ranking.c   # Exibição do ranking
│   └── ranking.c        # Persistência do ranking em disco
├── includes/            # Headers (.h)
├── assets/
│   ├── tela_inicial.png # Tela inicial
│   ├── tela_intro.png   # Tela de introdução (ponte)
│   ├── tela_frente.png  # Fachada do cinema
│   ├── My_Mirror_Image.wav  # Música do menu
│   ├── Haunting.wav     # Música do gameplay
│   ├── Ghost_Appear.wav # Música da perseguição
│   ├── Victory.mp3      # Música de vitória
│   └── Defeat.ogg       # Música de derrota
├── ranking.txt          # (gerado em runtime) — top 10 melhores tempos
└── Makefile
```

---

## Áudio

| Arquivo | Quando toca |
|---|---|
| `assets/My_Mirror_Image.wav` | Tela inicial e ao retornar dos fins de jogo |
| `assets/Haunting.wav` | Intro, entrada e gameplay enquanto não houver fantasma ativo |
| `assets/Ghost_Appear.wav` | Em loop enquanto houver pelo menos um fantasma ativo na sala |
| `assets/Victory.mp3` | Telas de cadastro de nome e ranking após vitória |
| `assets/Defeat.ogg` | Tela de Game Over |
