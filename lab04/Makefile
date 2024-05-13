CC = gcc
CFLAGS = -Wall -Wextra -std=c99

.PHONY: all clean

all: 

	$(CC) $(CFLAGS) -c 04_01_multi_thread.c
	$(CC) $(CFLAGS) -c 04_02_popup_thread.c
	$(CC) $(CFLAGS) -c 04_03_producer_consumer.c
	
	$(CC) $(CFLAGS) -o 04_01_multi_thread 04_01_multi_thread.o -lpthread
	$(CC) $(CFLAGS) -o 04_02_popup_thread 04_02_popup_thread.o -lpthread
	$(CC) $(CFLAGS) -o 04_03_producer_consumer 04_03_producer_consumer.o -lpthread

clean:
	rm -f 04_01_multi_thread 04_01_multi_thread.o
	rm -f 04_02_popup_thread 04_02_popup_thread.o
	rm -f 04_03_producer_consumer 04_03_producer_consumer.o
