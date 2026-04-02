#Сборка программы, линковка
program: main.o curses.o
		gcc main.o curses.o -lncurses -o program


#Cборка объектных модулей
curses.o: curses.c
		gcc -c curses.c -o curses.o
main.o: main.c
		gcc -c main.c -o main.o
#Очистка
.PHONY: clean

clean:
		rm -rf *.o program