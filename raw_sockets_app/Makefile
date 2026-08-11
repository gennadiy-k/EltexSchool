CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

.PHONY: all clean

all: $(BIN_DIR)/server $(BIN_DIR)/client

# Сборка исполняемых файлов
$(BIN_DIR)/server: $(OBJ_DIR)/server.o $(OBJ_DIR)/clients_list.o
	mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@

$(BIN_DIR)/client: $(OBJ_DIR)/client.o
	mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@

# Сборка объектных модулей
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Очистка
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
