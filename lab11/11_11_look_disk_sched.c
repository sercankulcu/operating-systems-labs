#include <stdio.h>
#include <stdlib.h>

#define MAX_REQUESTS 100

// Function to simulate LOOK disk arm scheduling
void look_disk_scheduling(int requests[], int num_requests, int initial_position) {
    int current_position = initial_position;
    int total_head_movement = 0;

    printf("LOOK Disk Arm Scheduling\n");

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
        if (requests[i] >= current_position) {
            int distance = abs(requests[i] - current_position);
            total_head_movement += distance;
            current_position = requests[i];

            printf("Moved to track %d, Head movement: %d\n", current_position, distance);
        }
    }

    // Process requests in the reverse direction
    for (int i = num_requests - 1; i >= 0; i--) {
        if (requests[i] < initial_position) {
            int distance = abs(requests[i] - current_position);
            total_head_movement += distance;
            current_position = requests[i];

            printf("Moved to track %d, Head movement: %d\n", current_position, distance);
        }
    }

    printf("Total Head Movement: %d\n", total_head_movement);
}

int main() {

    int requests[MAX_REQUESTS] = { 98, 183, 37, 122, 14, 124, 65, 67};
    int num_requests = 8;
    int initial_position = 53;

    // Simulate LOOK disk arm scheduling
    look_disk_scheduling(requests, num_requests, initial_position);

    return 0;
}
