TARGET = jogo
CC = gcc
CFLAGS = -I includes -Wall
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
SRC = $(wildcard src/*.c)

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LIBS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)