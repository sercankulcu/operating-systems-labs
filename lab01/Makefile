CC = gcc
CFLAGS = -Wall -Wextra -std=c99

.PHONY: all clean

all: 

	$(CC) $(CFLAGS) -c 01_01_greeting.c
	$(CC) $(CFLAGS) -c 01_02_square.c
	
	$(CC) $(CFLAGS) -o 01_01_greeting 01_01_greeting.o
	$(CC) $(CFLAGS) -o 01_02_square 01_02_square.o

clean:
	rm -f 01_01_greeting 01_01_greeting.o
	rm -f 01_02_square 01_02_square.o
