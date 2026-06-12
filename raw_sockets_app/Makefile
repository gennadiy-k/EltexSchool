TARGET1 = server
TARGET2 = client

all: $(TARGET1) $(TARGET2)

# Сборка программы 
$(TARGET1): server.o clients_list.o
	gcc server.o clients_list.o -o $(TARGET1)

$(TARGET2): client.o
	gcc client.o -o $(TARGET2)

# Сборка объектных модулей
server.o: server.c socket_in.h clients_list.h
	gcc -c server.c -o server.o
		
client.o: client.c socket_in.h
	gcc -c client.c -o client.o

clients_list.o: clients_list.c clients_list.h socket_in.h
	gcc -c clients_list.c -o clients_list.o

# Запуск
run: all
	sudo ./$(TARGET1)

# Очистка
.PHONY: all clean

clean:
	rm -rf *.o $(TARGET1) $(TARGET2)
