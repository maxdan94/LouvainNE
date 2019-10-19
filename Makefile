CC=gcc
CFLAGS=-O3
EXEC=recpart hi2vec renum

all: $(EXEC)

recpart: partition.o
	$(CC) -o recpart partition.o recpart.c $(CFLAGS)

partition.o: partition.c
	$(CC) -o partition.o -c partition.c $(CFLAGS)

hi2vec: 
	$(CC) -o hi2vec hi2vec.c $(CFLAGS) -lm

renum: 
	$(CC) -o renum renum.c $(CFLAGS)

clean:
	rm *.o recpart hi2vec renum
