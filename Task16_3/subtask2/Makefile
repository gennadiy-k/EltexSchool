TARGET1 = server
TARGET2 = client

all: $(TARGET1) $(TARGET2)

#Сборка программы
$(TARGET1): server.o
		gcc server.o -o $(TARGET1)

$(TARGET2): client.o
		gcc client.o -o $(TARGET2)

#Сборка объектных модулей
server.o: server.c socket_in.h
		gcc -c server.c -o server.o
		
client.o: client.c socket_in.h
		gcc -c client.c -o client.o

#Запуск
run: all
	-killall -9 client
	./$(TARGET2) &
	sleep 1
	./$(TARGET1)

#Очистка
.PHONY: all clean run

clean:
		rm -rf *.o $(TARGET1) $(TARGET2)