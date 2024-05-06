CC = gcc
CFLAGS = -Wall -Wextra -std=c99

.PHONY: all clean

all: 

	$(CC) $(CFLAGS) -c 07_01_dining_philo.c
	$(CC) $(CFLAGS) -c 07_02_reader_writer.c
	$(CC) $(CFLAGS) -c 07_03_bankers_multi.c
	$(CC) $(CFLAGS) -c 07_04_bankers_single.c
	
	$(CC) $(CFLAGS) -o 07_01_dining_philo 07_01_dining_philo.o -lpthread
	$(CC) $(CFLAGS) -o 07_02_reader_writer 07_02_reader_writer.o -lpthread
	$(CC) $(CFLAGS) -o 07_03_bankers_multi 07_03_bankers_multi.o -lpthread
	$(CC) $(CFLAGS) -o 07_04_bankers_single 07_04_bankers_single.o -lpthread

clean:
	rm -f 07_01_dining_philo 07_01_dining_philo.o
	rm -f 07_02_reader_writer 07_02_reader_writer.o
	rm -f 07_03_bankers_multi 07_03_bankers_multi.o
	rm -f 07_04_bankers_single 07_04_bankers_single.o
