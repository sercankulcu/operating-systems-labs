#include <stdio.h>
#include <stdlib.h>

#define MAX_REQUESTS 100

// Function to sort requests in FCFS order
void fcfs_sort(int requests[], int num_requests) {
    // FCFS scheduling does not require sorting
}

// Function to simulate FCFS disk arm scheduling
void fcfs_disk_scheduling(int requests[], int num_requests, int initial_position) {
    int current_position = initial_position;
    int total_head_movement = 0;

    printf("FCFS Disk Arm Scheduling\n");

    // Process requests in the order they arrive
    for (int i = 0; i < num_requests; i++) {
        int distance = abs(requests[i] - current_position); // Calculate distance to the requested track
        total_head_movement += distance; // Accumulate head movement
        current_position = requests[i]; // Move the disk arm to the requested track
        printf("Moved to track %d, Head movement: %d\n", current_position, distance);
    }

    printf("Total Head Movement: %d\n", total_head_movement);
}

int main() {
    int requests[MAX_REQUESTS] = { 98, 183, 37, 122, 14, 124, 65, 67};
    int num_requests = 8;
    int initial_position = 53;

    // Sort requests in FCFS order (not needed for FCFS scheduling)
    fcfs_sort(requests, num_requests);

    // Simulate FCFS disk arm scheduling
    fcfs_disk_scheduling(requests, num_requests, initial_position);

    return 0;
}

