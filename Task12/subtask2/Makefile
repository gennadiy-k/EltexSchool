TARGET1 = client
TARGET2 = server

all: $(TARGET1) $(TARGET2)

#Сборка программы
$(TARGET1): client.o
		gcc client.o -o $(TARGET1)
$(TARGET2): server.o
		gcc server.o -o $(TARGET2)

#Сборка объектных модулей
server.o: server.c
		gcc -c server.c -o server.o
client.o: client.c
		gcc -c client.c -o client.o

run: all
	./$(TARGET2)

#Очистка
.PHONY: all clean

clean:
		rm -rf *.o $(TARGET1) $(TARGET2) fifo1