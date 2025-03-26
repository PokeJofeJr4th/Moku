build: moku.c moku.h persistence.o food.o rsv.o helper.o
	gcc -g moku.c food.o persistence.o rsv.o helper.o -Wall -o moku.exe

food.o: food.c food.h
	gcc -g -c food.c -Wall -o food.o

persistence.o: persistence.c persistence.h rsv.h
	gcc -g -c persistence.c -Wall -o persistence.o

helper.o: helper.c helper.h
	gcc -g -c helper.c -Wall -o helper.o

test: unittest.c unittest.h food.o helper.o
	gcc -g unittest.c unittest.o food.o helper.o -Wall -o unittest.exe
	./unittest.exe