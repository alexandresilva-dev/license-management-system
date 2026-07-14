CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
TARGET = sistema_licencas

SRC = src/main.c src/utilitarios.c src/utilizadores.c src/licencas.c src/atribuicoes.c src/relatorios.c src/renovacoes.c

.PHONY: all run clean

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
