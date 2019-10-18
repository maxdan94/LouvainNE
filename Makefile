CC=gcc
CFLAGS=-O3
EXEC=main

all: $(EXEC)

main: partition.o main.o
	$(CC) -o recpart partition.o main.o $(CFLAGS)

partition.o: partition.c
	$(CC) -o partition.o -c partition.c $(CFLAGS)

main.o: main.c
	$(CC) -o main.o -c main.c $(CFLAGS)

clean:
	rm *.o recpart

