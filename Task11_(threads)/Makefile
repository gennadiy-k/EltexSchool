#Сборка программы, линковка
prog: main.o funcs.o
		gcc main.o funcs.o -pthread -o prog

#Cборка объектных модулей
funcs.o: funcs.c headers.h
		gcc -c funcs.c -o funcs.o
main.o: main.c headers.h
		gcc -c main.c -o main.o

run: prog
		./prog

#Очистка
.PHONY: clean

clean:
		rm -rf *.o prog