CC = gcc
CFLAGS = -Wall -std=c11 -g -lpthread

all: data task

data: gol_data.c
	$(CC) $(CFLAGS) gol_data.c -o gol_data

task: gol_task.c
	$(CC) $(CFLAGS) gol_task.c -o gol_task
