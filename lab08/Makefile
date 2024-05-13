CC = gcc
CFLAGS = -Wall -Wextra -std=c99

.PHONY: all clean

all: 

	$(CC) $(CFLAGS) -c 08_01_mmu.c
	$(CC) $(CFLAGS) -c 08_02_base_limit.c
	$(CC) $(CFLAGS) -c 08_03_first_fit.c
	$(CC) $(CFLAGS) -c 08_04_next_fit.c
	$(CC) $(CFLAGS) -c 08_05_best_fit.c
	$(CC) $(CFLAGS) -c 08_06_worst_fit.c
	$(CC) $(CFLAGS) -c 08_07_quick_fit.c
	$(CC) $(CFLAGS) -c 08_08_bitmap.c
	
	$(CC) $(CFLAGS) -o 08_01_mmu 08_01_mmu.o
	$(CC) $(CFLAGS) -o 08_02_base_limit 08_02_base_limit.o
	$(CC) $(CFLAGS) -o 08_03_first_fit 08_03_first_fit.o
	$(CC) $(CFLAGS) -o 08_04_next_fit 08_04_next_fit.o
	$(CC) $(CFLAGS) -o 08_05_best_fit 08_05_best_fit.o
	$(CC) $(CFLAGS) -o 08_06_worst_fit 08_06_worst_fit.o
	$(CC) $(CFLAGS) -o 08_07_quick_fit 08_07_quick_fit.o
	$(CC) $(CFLAGS) -o 08_08_bitmap 08_08_bitmap.o

clean:
	rm -f 08_01_mmu 08_01_mmu.o
	rm -f 08_02_base_limit 08_02_base_limit.o
	rm -f 08_03_first_fit 08_03_first_fit.o
	rm -f 08_04_next_fit 08_04_next_fit.o
	rm -f 08_05_best_fit 08_05_best_fit.o
	rm -f 08_06_worst_fit 08_06_worst_fit.o
	rm -f 08_07_quick_fit 08_07_quick_fit.o
	rm -f 08_08_bitmap 08_08_bitmap.o
