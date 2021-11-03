CC = gcc
CFLAGS = -Wall -std=c11 -g

all: encrypt decrypt

encrypt: a2encrypt.c
	$(CC) $(CFLAGS) a2encrypt.c -o a2encrypt

decrypt: a2decrypt.c
	mpicc $(CFLAGS) a2decrypt.c -o a2decrypt

clean:
	rm a2encrypt a2decrypt ciphertext.txt
