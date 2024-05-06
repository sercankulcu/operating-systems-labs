CC = gcc
CFLAGS = -Wall -Wextra -std=c99

.PHONY: all clean

all: 

	$(CC) $(CFLAGS) -c 02_01_read_system_call.c
	$(CC) $(CFLAGS) -c 02_02_write_system_call.c
	$(CC) $(CFLAGS) -c 02_03_process_system_call.c
	$(CC) $(CFLAGS) -c 02_04_math_functions.c
	$(CC) $(CFLAGS) -c 02_04_static_library.c
	$(CC) -c -fPIC 02_05_string_functions.c
	$(CC) $(CFLAGS) -c 02_05_shared_library.c
	
	$(CC) $(CFLAGS) -o 02_01_read_system_call 02_01_read_system_call.o
	$(CC) $(CFLAGS) -o 02_02_write_system_call 02_02_write_system_call.o
	$(CC) $(CFLAGS) -o 02_03_process_system_call 02_03_process_system_call.o
	ar rcs libmymathlib.a 02_04_math_functions.o
	$(CC) $(CFLAGS) -o 02_04_static_library 02_04_static_library.o -L. -lmymathlib
	gcc -shared -o libmystringlib.so 02_05_string_functions.o
	$(CC) $(CFLAGS) -o 02_05_shared_library 02_05_shared_library.o -L. -lmystringlib
	sudo cp libmystringlib.so /usr/lib

clean:
	rm -f 02_01_read_system_call 02_01_read_system_call.o
	rm -f 02_02_write_system_call 02_02_write_system_call.o
	rm -f 02_03_process_system_call 02_03_process_system_call.o
	rm -f 02_04_math_functions.o
	rm -f 02_04_static_library 02_04_static_library.o
	rm -f libmymathlib.a
	rm -f 02_05_string_functions.o
	rm -f 02_05_shared_library 02_05_shared_library.o
	rm -f libmystringlib.so
	sudo rm -f /usr/lib/libmystringlib.so
