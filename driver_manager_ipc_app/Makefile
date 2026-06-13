TARGET = cli_app

all: $(TARGET)

# Сборка программы 
$(TARGET): cli_main.o driver.o cli_manager.o
	gcc cli_main.o driver.o cli_manager.o -o $(TARGET)

# Сборка объектных модулей
cli_main.o: cli_main.c cmn_types.h driver.h cli_manager.h
	gcc -c cli_main.c -o cli_main.o

driver.o: driver.c cmn_types.h driver.h
	gcc -c driver.c -o driver.o

cli_manager.o: cli_manager.c cmn_types.h cli_manager.h
	gcc -c cli_manager.c -o cli_manager.o

# Запуск
run: all
	./$(TARGET)

# Очистка
.PHONY: all clean

clean:
	rm -rf *.o $(TARGET)
