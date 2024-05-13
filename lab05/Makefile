CC = gcc
CFLAGS = -Wall -Wextra -std=c99

.PHONY: all clean

all: 

	$(CC) $(CFLAGS) -c 05_01_posix_sched.c
	$(CC) $(CFLAGS) -c 05_02_fcfs_sched.c
	$(CC) $(CFLAGS) -c 05_03_sjf_sched.c
	$(CC) $(CFLAGS) -c 05_04_srtn_sched.c
	$(CC) $(CFLAGS) -c 05_05_round_robin.c
	$(CC) $(CFLAGS) -c 05_06_priority_sched.c
	$(CC) $(CFLAGS) -c 05_07_multi_queues.c
	$(CC) $(CFLAGS) -c 05_08_spn_sched.c
	$(CC) $(CFLAGS) -c 05_09_gmr_sched.c
	$(CC) $(CFLAGS) -c 05_10_lottery_sched.c
	$(CC) $(CFLAGS) -c 05_11_fair_share_sched.c
	
	$(CC) $(CFLAGS) -o 05_01_posix_sched 05_01_posix_sched.o -lpthread
	$(CC) $(CFLAGS) -o 05_02_fcfs_sched 05_02_fcfs_sched.o 
	$(CC) $(CFLAGS) -o 05_03_sjf_sched 05_03_sjf_sched.o 
	$(CC) $(CFLAGS) -o 05_04_srtn_sched 05_04_srtn_sched.o 
	$(CC) $(CFLAGS) -o 05_05_round_robin 05_05_round_robin.o 
	$(CC) $(CFLAGS) -o 05_06_priority_sched 05_06_priority_sched.o 
	$(CC) $(CFLAGS) -o 05_07_multi_queues 05_07_multi_queues.o 
	$(CC) $(CFLAGS) -o 05_08_spn_sched 05_08_spn_sched.o 
	$(CC) $(CFLAGS) -o 05_09_gmr_sched 05_09_gmr_sched.o 
	$(CC) $(CFLAGS) -o 05_10_lottery_sched 05_10_lottery_sched.o 
	$(CC) $(CFLAGS) -o 05_11_fair_share_sched 05_11_fair_share_sched.o 

clean:
	rm -f  05_01_posix_sched 05_01_posix_sched.o
	rm -f  05_02_fcfs_sched 05_02_fcfs_sched.o
	rm -f  05_03_sjf_sched 05_03_sjf_sched.o
	rm -f  05_04_srtn_sched 05_04_srtn_sched.o
	rm -f  05_05_round_robin 05_05_round_robin.o
	rm -f  05_06_priority_sched 05_06_priority_sched.o
	rm -f  05_07_multi_queues 05_07_multi_queues.o
	rm -f  05_08_spn_sched 05_08_spn_sched.o
	rm -f  05_09_gmr_sched 05_09_gmr_sched.o
	rm -f  05_10_lottery_sched 05_10_lottery_sched.o
	rm -f  05_11_fair_share_sched 05_11_fair_share_sched.o
