#include <stdio.h>

// Process structure
struct Process {
    int id;      // Process ID
    int burst;   // Burst time
    int arrival; // Arrival time
};

// Function to compute waiting time for each process
void findWaitingTime(struct Process processes[], int n, int wt[]) {
    wt[0] = 0;
    for (int i = 1; i < n; i++) {
        wt[i] = processes[i - 1].burst + wt[i - 1];
    }
}

// Function to compute turnaround time for each process
void findTurnAroundTime(struct Process processes[], int n, int wt[], int tat[]) {
    for (int i = 0; i < n; i++) {
        tat[i] = processes[i].burst + wt[i];
    }
}

// Function to calculate average time
void findAverageTime(struct Process processes[], int n) {
    int wt[n], tat[n], total_wt = 0, total_tat = 0;

    // Calculate waiting time for each process
    findWaitingTime(processes, n, wt);

    // Calculate turnaround time for each process
    findTurnAroundTime(processes, n, wt, tat);

    // Display processes along with all details
    printf("Process\tBurst Time\tArrival Time\tWaiting Time\tTurnaround Time\n");

    // Calculate total waiting time and turnaround time
    for (int i = 0; i < n; i++) {
        total_wt += wt[i];
        total_tat += tat[i];
        printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\n", processes[i].id, processes[i].burst, processes[i].arrival, wt[i], tat[i]);
    }

    // Calculate and display average waiting time and turnaround time
    printf("Average waiting time = %.2f\n", (float)total_wt / n);
    printf("Average turnaround time = %.2f\n", (float)total_tat / n);
}

int main() {
    // Number of processes
    int n = 5;

    // Array to store processes
    struct Process processes[n];

    // Input process details
    processes[0].id = 1;
    processes[0].burst = 10;
    processes[0].arrival = 1;
    processes[1].id = 2;
    processes[1].burst = 8;
    processes[1].arrival = 2;
    processes[2].id = 3;
    processes[2].burst = 6;
    processes[2].arrival = 3;
    processes[3].id = 4;
    processes[3].burst = 4;
    processes[3].arrival = 4;
    processes[4].id = 5;
    processes[4].burst = 2;
    processes[4].arrival = 5;

    // Calculate and display average waiting time and turnaround time
    findAverageTime(processes, n);

    return 0;
}

