CC = gcc
CFLAGS = -Wall -std=c11 -g

all: serial

serial: maze.c
	$(CC) $(CFLAGS) maze.c -o maze

parallel: maze.c
		$(CC) $(CFLAGS) -fopenmp -o mazep maze.c

clean:
	rm maze mazep
