#include <stdio.h>
#include <stdlib.h>

#define MAX_REQUESTS 100

// Function to simulate C-SCAN disk arm scheduling
void cscan_disk_scheduling(int requests[], int num_requests, int initial_position, int max_track) {
    int current_position = initial_position;
    int total_head_movement = 0;
    int visited[MAX_REQUESTS] = {0}; // Array to keep track of visited requests

    printf("C-SCAN Disk Arm Scheduling\n");

    // Sort requests in ascending order
    for (int i = 0; i < num_requests - 1; i++) {
        for (int j = 0; j < num_requests - i - 1; j++) {
            if (requests[j] > requests[j + 1]) {
                int temp = requests[j];
                requests[j] = requests[j + 1];
                requests[j + 1] = temp;
            }
        }
    }

    // Process requests in the forward direction
    for (int i = 0; i < num_requests; i++) {
        if (requests[i] >= current_position && !visited[i]) {
            int distance = abs(requests[i] - current_position);
            total_head_movement += distance;
            current_position = requests[i];
            visited[i] = 1;

            printf("Moved to track %d, Head movement: %d\n", current_position, distance);
        }
    }

    // Move to the beginning of the disk
    total_head_movement += max_track - current_position;
    total_head_movement += max_track;
    current_position = 0;

    // Process remaining requests in the forward direction
    for (int i = 0; i < num_requests; i++) {
        if (requests[i] < initial_position && !visited[i]) {
            int distance = abs(requests[i] - current_position);
            total_head_movement += distance;
            current_position = requests[i];
            visited[i] = 1;

            printf("Moved to track %d, Head movement: %d\n", current_position, distance);
        }
    }

    printf("Total Head Movement: %d\n", total_head_movement);
}

int main() {

    int requests[MAX_REQUESTS] = { 98, 183, 37, 122, 14, 124, 65, 67};
    int num_requests = 8;
    int initial_position = 53;
    int max_track = 200;

    // Simulate C-SCAN disk arm scheduling
    cscan_disk_scheduling(requests, num_requests, initial_position, max_track);

    return 0;
}

