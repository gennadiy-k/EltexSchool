CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

.PHONY: all clean

all: $(BIN_DIR)/cli_app

# Сборка исполняемого файла
$(BIN_DIR)/cli_app: $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@

# Сборка объектных модулей
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Запуск
run: all
	$(BIN_DIR)/cli_app

# Очистка
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
