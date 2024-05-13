CC = gcc
CFLAGS = -Wall -Wextra -std=c99

.PHONY: all clean

all: 

	$(CC) $(CFLAGS) -c 10_01_character.c
	$(CC) $(CFLAGS) -c 10_02_block.c
	$(CC) $(CFLAGS) -c 10_03_directory_ops.c
	$(CC) $(CFLAGS) -c 10_04_file_ops.c
	$(CC) $(CFLAGS) -c 10_05_call_executable.c
	$(CC) $(CFLAGS) -c 10_06_file_stat.c
	$(CC) $(CFLAGS) -c 10_07_file_copy.c
	
	$(CC) $(CFLAGS) -o 10_01_character 10_01_character.o
	$(CC) $(CFLAGS) -o 10_02_block 10_02_block.o
	$(CC) $(CFLAGS) -o 10_03_directory_ops 10_03_directory_ops.o
	$(CC) $(CFLAGS) -o 10_04_file_ops 10_04_file_ops.o
	cp 10_01_character external_executable
	$(CC) $(CFLAGS) -o 10_05_call_executable 10_05_call_executable.o
	$(CC) $(CFLAGS) -o 10_06_file_stat 10_06_file_stat.o
	$(CC) $(CFLAGS) -o 10_07_file_copy 10_07_file_copy.o

clean:
	rm -f 10_01_character 10_01_character.o test.txt
	rm -f 10_02_block 10_02_block.o block_file.bin
	rm -f 10_03_directory_ops 10_03_directory_ops.o
	rm -f 10_04_file_ops 10_04_file_ops.o example_file.txt
	rm -f 10_05_call_executable 10_05_call_executable.o external_executable
	rm -f 10_06_file_stat 10_06_file_stat.o
	rm -f 10_07_file_copy 10_07_file_copy.o

