CC = gcc
CFLAGS = -Wall -Wextra -std=c99

.PHONY: all clean

all: 

	$(CC) $(CFLAGS) -c 03_01_shared_memory.c
	$(CC) $(CFLAGS) -c 03_02_message_queue.c
	$(CC) $(CFLAGS) -c 03_03_peterson.c
	$(CC) $(CFLAGS) -c 03_04_pipe.c
	
	$(CC) $(CFLAGS) -o 03_01_shared_memory 03_01_shared_memory.o
	$(CC) $(CFLAGS) -o 03_02_message_queue 03_02_message_queue.o
	$(CC) $(CFLAGS) -o 03_03_peterson 03_03_peterson.o -lpthread
	$(CC) $(CFLAGS) -o 03_04_pipe 03_04_pipe.o

clean:
	rm -f 03_01_shared_memory 03_01_shared_memory.o
	rm -f 03_02_message_queue 03_02_message_queue.o
	rm -f 03_03_peterson 03_03_peterson.o
	rm -f 03_04_pipe 03_04_pipe.o
