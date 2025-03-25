build: moku.c rsv.o moku.h food.c persistence.c rsv.h
	gcc -g moku.c rsv.o -Wall -o moku.exe