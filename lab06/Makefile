CC = gcc
CFLAGS = -Wall -Wextra -std=c99

.PHONY: all clean

all: 

	$(CC) $(CFLAGS) -c 06_01_mutex.c
	$(CC) $(CFLAGS) -c 06_02_strict_alternation.c
	$(CC) $(CFLAGS) -c 06_03_busy_waiting.c
	$(CC) $(CFLAGS) -c 06_04_wait_signal.c
	$(CC) $(CFLAGS) -c 06_05_semaphore.c
	
	$(CC) $(CFLAGS) -o 06_01_mutex 06_01_mutex.o -lpthread
	$(CC) $(CFLAGS) -o 06_02_strict_alternation 06_02_strict_alternation.o -lpthread
	$(CC) $(CFLAGS) -o 06_03_busy_waiting 06_03_busy_waiting.o -lpthread
	$(CC) $(CFLAGS) -o 06_04_wait_signal 06_04_wait_signal.o -lpthread
	$(CC) $(CFLAGS) -o 06_05_semaphore 06_05_semaphore.o -lpthread

clean:
	rm -f 06_01_mutex 06_01_mutex.o
	rm -f 06_02_strict_alternation 06_02_strict_alternation.o
	rm -f 06_03_busy_waiting 06_03_busy_waiting.o
	rm -f 06_04_wait_signal 06_04_wait_signal.o
	rm -f 06_05_semaphore 06_05_semaphore.o
