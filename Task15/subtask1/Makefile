TARGET1 = sigaction
TARGET2 = sigsnd

all: $(TARGET1) $(TARGET2)

#Сборка программы
$(TARGET1): sigaction.o
		gcc sigaction.o -o $(TARGET1)

$(TARGET2): sigsnd.o
		gcc sigsnd.o -o $(TARGET2)

#Сборка объектных модулей
sigaction.o: sigaction.c 
		gcc -c sigaction.c -o sigaction.o
		
sigsnd.o: sigsnd.c 
		gcc -c sigsnd.c -o sigsnd.o

#Запуск
run: all
	./$(TARGET1)

#Очистка
.PHONY: all clean run

clean:
		rm -rf *.o $(TARGET1) $(TARGET2)