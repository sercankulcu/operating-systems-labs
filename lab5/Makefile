CC = gcc
CFLAGS = -Wall -Wextra -std=c99

.PHONY: all clean

all: 

	$(CC) $(CFLAGS) -c 05_01_posix_sched.c
	
	$(CC) $(CFLAGS) -o 05_01_posix_sched 05_01_posix_sched.o -lpthread

clean:
	rm -f 05_01_posix_sched 05_01_posix_sched.o
