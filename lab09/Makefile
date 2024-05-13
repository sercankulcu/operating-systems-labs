CC = gcc
CFLAGS = -Wall -Wextra -std=c99

.PHONY: all clean

all: 

	$(CC) $(CFLAGS) -c 09_01_optimal.c
	$(CC) $(CFLAGS) -c 09_02_not_recently_used.c
	$(CC) $(CFLAGS) -c 09_03_first_in_first_out.c
	$(CC) $(CFLAGS) -c 09_04_second_chance.c
	$(CC) $(CFLAGS) -c 09_05_clock.c
	$(CC) $(CFLAGS) -c 09_06_least_recently_used.c
	$(CC) $(CFLAGS) -c 09_07_working_set.c
	$(CC) $(CFLAGS) -c 09_08_working_set_clock.c
	
	$(CC) $(CFLAGS) -o 09_01_optimal 09_01_optimal.o
	$(CC) $(CFLAGS) -o 09_02_not_recently_used 09_02_not_recently_used.o
	$(CC) $(CFLAGS) -o 09_03_first_in_first_out 09_03_first_in_first_out.o
	$(CC) $(CFLAGS) -o 09_04_second_chance 09_04_second_chance.o
	$(CC) $(CFLAGS) -o 09_05_clock 09_05_clock.o
	$(CC) $(CFLAGS) -o 09_06_least_recently_used 09_06_least_recently_used.o
	$(CC) $(CFLAGS) -o 09_07_working_set 09_07_working_set.o
	$(CC) $(CFLAGS) -o 09_08_working_set_clock 09_08_working_set_clock.o

clean:
	rm -f 09_01_optimal 09_01_optimal.o
	rm -f 09_02_not_recently_used 09_02_not_recently_used.o
	rm -f 09_03_first_in_first_out 09_03_first_in_first_out.o
	rm -f 09_04_second_chance 09_04_second_chance.o
	rm -f 09_05_clock 09_05_clock.o
	rm -f 09_06_least_recently_used 09_06_least_recently_used.o
	rm -f 09_07_working_set 09_07_working_set.o
	rm -f 09_08_working_set_clock 09_08_working_set_clock.o
