CC = gcc
CFLAGS = -Wall -std=c11 -g

all: encrypt

encrypt: a2encrypt.c
	$(CC) $(CFLAGS) a2encrypt.c -o a2encrypt

clean:
	rm a2encrypt ciphertext.txt
