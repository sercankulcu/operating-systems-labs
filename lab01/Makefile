CC = gcc
CFLAGS = -Wall -Wextra -std=c99

.PHONY: all clean

all: 

	gcc -S 01_01_greeting.c
	$(CC) $(CFLAGS) -c 01_01_greeting.c
	$(CC) $(CFLAGS) -c 01_02_square.c
	$(CC) $(CFLAGS) -c 01_03_arguments.c
	$(CC) $(CFLAGS) -c 01_04_loop.c
	
	$(CC) $(CFLAGS) -o 01_01_greeting 01_01_greeting.o
	$(CC) $(CFLAGS) -o 01_02_square 01_02_square.o
	$(CC) $(CFLAGS) -o 01_03_arguments 01_03_arguments.o
	$(CC) $(CFLAGS) -o 01_04_loop 01_04_loop.o

clean:
	rm -f 01_01_greeting 01_01_greeting.o 01_01_greeting.s
	rm -f 01_02_square 01_02_square.o
	rm -f 01_03_arguments 01_03_arguments.o
	rm -f 01_04_loop 01_04_loop.o
