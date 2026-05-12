TARGET = prog

all: $(TARGET)

#Сборка программы
$(TARGET): main.o
		gcc main.o -o $(TARGET)

#Сборка объектных модулей
main.o: main.c
		gcc -c main.c -o main.o

run: $(TARGET)
		./$(TARGET)

#Очистка
.PHONY: all clean

clean:
		rm -rf *.o $(TARGET)