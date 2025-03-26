build: moku.c moku.h persistence.o food.o rsv.o
	gcc -g moku.c food.o persistence.o rsv.o -Wall -o moku.exe

food.o: food.c food.h
	gcc -g -c food.c -Wall -o food.o

persistence.o: persistence.c persistence.h rsv.h
	gcc -g -c persistence.c -Wall -o persistence.o