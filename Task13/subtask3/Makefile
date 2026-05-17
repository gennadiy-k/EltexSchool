TARGET1 = server
TARGET2 = client

OBJS_CLIENT = client.o gui.o client_rcv_func.o
OBJS_SERVER = server.o

all: $(TARGET1) $(TARGET2)

#Сборка программы
$(TARGET1): $(OBJS_SERVER)
		gcc $(OBJS_SERVER) -o $(TARGET1) 

$(TARGET2): $(OBJS_CLIENT)
		gcc $(OBJS_CLIENT) -o $(TARGET2) -lncurses -lpthread


#Сборка объектных модулей
server.o: server.c message.h gui.h
		gcc -c server.c -o server.o

client.o: client.c message.h gui.h
		gcc -c client.c -o client.o

gui.o: gui.c message.h gui.h
		gcc -c gui.c -o gui.o

client_rcv_func.o: client_rcv_func.c message.h gui.h
		gcc -c client_rcv_func.c -o client_rcv_func.o

#Запуск
run: all
		./$(TARGET1)

#Очистка
.PHONY: all clean run

clean:
		rm -rf *.o $(TARGET1) $(TARGET2)