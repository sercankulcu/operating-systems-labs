#include <stdio.h>
#include <stdlib.h>

#define SEGMENT_SIZE 100

// Structure to represent a segment
typedef struct {
    int *base; // Base address
    int limit; // Limit of the segment
} Segment;

// Function to initialize a segment
void initializeSegment(Segment *segment, int limit) {
    segment->base = (int *)malloc(limit * sizeof(int));
    segment->limit = limit;
}

// Function to read data from a segment
int readFromSegment(Segment *segment, int offset) {
    if (offset < segment->limit) {
        return segment->base[offset];
    } else {
        printf("Error: Accessing beyond segment limit\n");
        return -1;
    }
}

// Function to write data to a segment
void writeToSegment(Segment *segment, int offset, int data) {
    if (offset < segment->limit) {
        segment->base[offset] = data;
    } else {
        printf("Error: Accessing beyond segment limit\n");
    }
}

int main() {
    Segment segment;
    int offset = 50; // Offset within the segment

    // Initialize the segment with a limit
    initializeSegment(&segment, SEGMENT_SIZE);

    // Write data to the segment
    writeToSegment(&segment, offset, 42);

    // Read data from the segment
    int data = readFromSegment(&segment, offset);
    if (data != -1) {
        printf("Data read from segment: %d\n", data);
    }

    // Free the memory allocated for the segment
    free(segment.base);

    return 0;
}

/* We define a Segment structure to represent a segment of memory, containing a base address and a limit. We simulate the initialization, read, and write operations on segments. The SEGMENT_SIZE macro defines the size of the segment, which acts as the limit. We demonstrate writing and reading data to/from the segment. While this code doesn't directly manipulate base and limit registers, it provides a conceptual understanding of how they work by simulating their behavior through segmentation.
 * */
