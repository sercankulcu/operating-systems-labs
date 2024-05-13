CC = gcc
CFLAGS = -Wall -Wextra -std=c99

.PHONY: all clean

all: 

	$(CC) $(CFLAGS) -c 11_01_dma_controller.c
	$(CC) $(CFLAGS) -c 11_02_interrupt_controller.c
	$(CC) -c 11_03_timer.c
	$(CC) $(CFLAGS) -c 11_04_serial_port.c
	$(CC) $(CFLAGS) -c 11_05_parallel_port.c
	$(CC) $(CFLAGS) -c 11_07_fcfs_disk_sched.c
	$(CC) $(CFLAGS) -c 11_08_sstf_disk_sched.c
	$(CC) $(CFLAGS) -c 11_09_scan_disk_sched.c
	$(CC) $(CFLAGS) -c 11_10_cscan_disk_sched.c
	$(CC) $(CFLAGS) -c 11_11_look_disk_sched.c
	
	$(CC) $(CFLAGS) -o 11_01_dma_controller 11_01_dma_controller.o
	$(CC) $(CFLAGS) -o 11_02_interrupt_controller 11_02_interrupt_controller.o
	$(CC) $(CFLAGS) -o 11_03_timer 11_03_timer.o -lrt
	$(CC) $(CFLAGS) -o 11_04_serial_port 11_04_serial_port.o
	$(CC) $(CFLAGS) -o 11_05_parallel_port 11_05_parallel_port.o
	$(CC) $(CFLAGS) -o 11_07_fcfs_disk_sched 11_07_fcfs_disk_sched.o
	$(CC) $(CFLAGS) -o 11_08_sstf_disk_sched 11_08_sstf_disk_sched.o
	$(CC) $(CFLAGS) -o 11_09_scan_disk_sched 11_09_scan_disk_sched.o
	$(CC) $(CFLAGS) -o 11_10_cscan_disk_sched 11_10_cscan_disk_sched.o
	$(CC) $(CFLAGS) -o 11_11_look_disk_sched 11_11_look_disk_sched.o

clean:
	rm -f 11_01_dma_controller 11_01_dma_controller.o
	rm -f 11_02_interrupt_controller 11_02_interrupt_controller.o
	rm -f 11_03_timer 11_03_timer.o
	rm -f 11_04_serial_port 11_04_serial_port.o
	rm -f 11_05_parallel_port 11_05_parallel_port.o
	rm -f 11_06_device_driver_user 11_06_device_driver_user.o
	rm -f 11_07_fcfs_disk_sched 11_07_fcfs_disk_sched.o
	rm -f 11_08_sstf_disk_sched 11_08_sstf_disk_sched.o
	rm -f 11_09_scan_disk_sched 11_09_scan_disk_sched.o
	rm -f 11_10_cscan_disk_sched 11_10_cscan_disk_sched.o
	rm -f 11_11_look_disk_sched 11_11_look_disk_sched.o
