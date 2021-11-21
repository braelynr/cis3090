CC = gcc
CFLAGS = -Wall -std=c11 -g

all: serial parallel

serial: maze.c
	$(CC) $(CFLAGS) -fopenmp -o maze maze.c

parallel: maze.c
	$(CC) $(CFLAGS) -fopenmp -o mazep maze.c

clean:
	rm maze mazep
