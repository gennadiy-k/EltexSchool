TARGET1 = sigprocmask
TARGET2 = sigsnd

all: $(TARGET1) $(TARGET2)

#Сборка программы
$(TARGET1): sigprocmask.o
		gcc sigprocmask.o -o $(TARGET1)

$(TARGET2): sigsnd.o
		gcc sigsnd.o -o $(TARGET2)

#Сборка объектных модулей
sigprocmask.o: sigprocmask.c 
		gcc -c sigprocmask.c -o sigprocmask.o
		
sigsnd.o: sigsnd.c 
		gcc -c sigsnd.c -o sigsnd.o

#Запуск
run: all
	./$(TARGET1)

#Очистка
.PHONY: all clean run

clean:
		rm -rf *.o $(TARGET1) $(TARGET2)