TARGET  = jogo
CC      = gcc
CFLAGS  = -I includes -Wall -O2
LIBS    = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
SRC     = $(wildcard src/*.c)

.PHONY: all run clean deps check help

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LIBS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)

# Instala todas as dependencias (Ubuntu / Debian / WSL2)
deps:
	sudo apt update
	sudo apt install -y build-essential libraylib-dev \
	                    libgl1-mesa-dev libx11-dev libasound2-dev

# Verifica se as dependencias estao presentes antes de compilar
check:
	@printf "gcc    : "; command -v gcc    >/dev/null 2>&1 \
		&& echo "OK" \
		|| echo "FALTA  ->  sudo apt install build-essential"
	@printf "make   : "; command -v make   >/dev/null 2>&1 \
		&& echo "OK" \
		|| echo "FALTA  ->  sudo apt install build-essential"
	@printf "raylib : "; ldconfig -p 2>/dev/null | grep -q libraylib \
		&& echo "OK" \
		|| echo "FALTA  ->  make deps"
	@printf "libGL  : "; ldconfig -p 2>/dev/null | grep -q libGL \
		&& echo "OK" \
		|| echo "FALTA  ->  sudo apt install libgl1-mesa-dev"
	@printf "libX11 : "; ldconfig -p 2>/dev/null | grep -q libX11 \
		&& echo "OK" \
		|| echo "FALTA  ->  sudo apt install libx11-dev"

help:
	@echo ""
	@echo "=============================="
	@echo "  After Session — como rodar  "
	@echo "=============================="
	@echo ""
	@echo "1. Instalar dependencias (apenas na primeira vez):"
	@echo "     make deps"
	@echo ""
	@echo "2. Compilar:"
	@echo "     make"
	@echo ""
	@echo "3. Compilar e executar:"
	@echo "     make run"
	@echo ""
	@echo "4. Remover o executavel:"
	@echo "     make clean"
	@echo ""
	@echo "5. Verificar se as dependencias estao instaladas:"
	@echo "     make check"
	@echo ""
	@echo "Dependencias necessarias:"
	@echo "  gcc / build-essential"
	@echo "  raylib >= 4.x  (libraylib-dev)"
	@echo "  libGL / libX11 / libasound2  (normalmente ja instaladas)"
	@echo ""
	@echo "WSL2 + Windows 11 (WSLg):"
	@echo "  Funciona direto — so rodar 'make run'."
	@echo ""
	@echo "WSL2 sem WSLg (Windows 10):"
	@echo "  Instale o VcXsrv ou X410, inicie o servidor X e entao:"
	@echo "    export DISPLAY=:0"
	@echo "    make run"
	@echo ""
