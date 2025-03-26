build: moku.c rsv.o moku.h persistence.c rsv.h food.o
	gcc -g moku.c rsv.o food.o -Wall -o moku.exe

food.o: food.c food.h
	gcc -g -c food.c -Wall -o food.o