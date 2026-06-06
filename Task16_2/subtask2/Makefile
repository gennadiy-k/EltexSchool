TARGET1 = server
TARGET2 = client

all: $(TARGET1) $(TARGET2)

#Сборка программы
$(TARGET1): server.o service_thread.o
		gcc server.o service_thread.o -o $(TARGET1) -lpthread

$(TARGET2): client.o
		gcc client.o -o $(TARGET2) -lpthread

#Сборка объектных модулей
server.o: server.c socket_in.h
		gcc -c server.c -o server.o
		
client.o: client.c socket_in.h
		gcc -c client.c -o client.o

service_thread.o: service_thread.c socket_in.h
		gcc -c service_thread.c -o service_thread.o

#Запуск сервера
run: $(TARGET1)
	./$(TARGET1)

#Запуск теста клиентов
test_clients: $(TARGET2)
	./$(TARGET2)

#Очистка
.PHONY: all clean run test_clients

clean:
		rm -rf *.o $(TARGET1) $(TARGET2)