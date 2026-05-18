TARGET = jogo
CC = gcc
CFLAGS = -I -includes -Wall
LIBS = -lraylib -lGl -lm -lpthread -ldl -lrt -lX11
SRC = $(wildcard src/*.c)
all: $(TARGET)
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(CFLAGS) $(LIBS)
run: all
	./$(TARGET)
clean:
	rm -f $(TARGET)