#Переменные
LIB := libmy_calc.a
OBJ_M := add.o sub.o mul.o div.o

#Сборка программы, линковка
program: main.c $(LIB)
		gcc main.c -o program -L. -lmy_calc

#Сборка библиотеки
$(LIB): $(OBJ_M)
		ar rc $(LIB) $(OBJ_M)

#Cборка объектных модулей
add.o: add.c
		gcc add.c -c
sub.o: sub.c
		gcc sub.c -c
mul.o: mul.c
		gcc mul.c -c
div.o: div.c
		gcc div.c -c

#Очистка
.PHONY: clean

clean:
		rm -rf *.o *.a program

