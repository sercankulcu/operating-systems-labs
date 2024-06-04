#include <stdio.h>
#include <limits.h>

// Process structure
struct Process {
    int id;      // Process ID
    int burst;   // Burst time
    int arrival; // Arrival time
    int remaining; // Remaining burst time
};

// Function to find the process with the shortest remaining time
int findShortestProcess(struct Process processes[], int n, int currentTime) {
    int shortest = -1;
    int shortestTime = INT_MAX;

    for (int i = 0; i < n; i++) {
        if (processes[i].arrival <= currentTime && processes[i].remaining < shortestTime && processes[i].remaining > 0) {
            shortest = i;
            shortestTime = processes[i].remaining;
        }
    }

    return shortest;
}

// Function to perform SRTN scheduling
void srtNScheduling(struct Process processes[], int n) {
    int currentTime = 0;
    int completed = 0;
    int prevProcess = -1;

    while (completed != n) {
        int shortest = findShortestProcess(processes, n, currentTime);

        if (shortest == -1) {
            currentTime++;
        } else {
            // Execute the process with shortest remaining time
            if (prevProcess != shortest) {
                //printf("Process %d is running from time %d to ", processes[shortest].id, currentTime);
            }
            processes[shortest].remaining--;

            // If process is completed, update completion time and print
            if (processes[shortest].remaining == 0) {
                //printf("%d\n", currentTime + 1);
                completed++;
            }

            printf("Process %d is running from time %d to %d\n", processes[shortest].id, currentTime, currentTime+1);

            prevProcess = shortest;
            currentTime++;
        }
    }
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
    processes[0].remaining = processes[0].burst;
    processes[1].id = 2;
    processes[1].burst = 8;
    processes[1].arrival = 2;
    processes[1].remaining = processes[1].burst;
    processes[2].id = 3;
    processes[2].burst = 6;
    processes[2].arrival = 3;
    processes[2].remaining = processes[2].burst;
    processes[3].id = 4;
    processes[3].burst = 4;
    processes[3].arrival = 4;
    processes[3].remaining = processes[3].burst;
    processes[4].id = 5;
    processes[4].burst = 2;
    processes[4].arrival = 5;
    processes[4].remaining = processes[4].burst;

    // Display processes along with all details
    printf("Process\t\tBurst Time\tArrival Time\n");

    // Calculate total waiting time and turnaround time
    for (int i = 0; i < n; i++) {
        printf("%d\t\t%d\t\t%d\n", processes[i].id, processes[i].burst, processes[i].arrival);
    }

    // Perform SRTN scheduling
    printf("Sequence of processes execution:\n");
    srtNScheduling(processes, n);

    return 0;
}

