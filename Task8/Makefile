#Переменные
LIB := libmy_calc.so
OBJ_M := add.o sub.o mul.o div.o

#Сборка программы, линковка
program: main.c $(LIB)
		gcc main.c -o program -L. -lmy_calc -Wl,-rpath,.

#Сборка библиотеки
$(LIB): $(OBJ_M)
		gcc -shared $(OBJ_M) -o $(LIB)

#Cборка объектных модулей
add.o: add.c
		gcc add.c -fPIC -c
sub.o: sub.c
		gcc sub.c -fPIC -c
mul.o: mul.c
		gcc mul.c -fPIC -c
div.o: div.c
		gcc div.c -fPIC -c

#Очистка
.PHONY: clean

clean:
		rm -rf *.o *.so program

