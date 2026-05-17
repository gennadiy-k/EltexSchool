TARGET1 = client
TARGET2 = server

all: $(TARGET1) $(TARGET2)

#Сборка программы
$(TARGET1): client.o
		gcc client.o -o $(TARGET1) -lrt
$(TARGET2): server.o
		gcc server.o -o $(TARGET2) -lrt

#Сборка объектных модулей
server.o: server.c message.h
		gcc -c server.c -o server.o
client.o: client.c message.h
		gcc -c client.c -o client.o

#Запуск
run: all
	./$(TARGET2) &
	sleep 1
	./$(TARGET1)

#Очистка
.PHONY: all clean run

clean:
		rm -rf *.o $(TARGET1) $(TARGET2)