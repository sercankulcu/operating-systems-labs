// Include necessary headers
#include <stdio.h>      // For standard input/output functions
#include <stdlib.h>     // For memory allocation (malloc, free) and exit
#include <stdbool.h>

// Define constants
#define SEGMENT_SIZE 100    // Default size of each segment in integers
#define NUM_SEGMENTS 3      // Number of segments in the system

// Structure to represent a segment in memory
typedef struct {
    int *base;      // Pointer to the base address of the segment in memory
    int limit;      // Maximum size of the segment (in integers)
    int id;         // Unique identifier for the segment
} Segment;

// Function to initialize a segment with given limit and ID
void initializeSegment(Segment *segment, int limit, int id) {
    // Allocate memory for the segment
    segment->base = (int *)malloc(limit * sizeof(int));
    if (segment->base == NULL) {  // Check for allocation failure
        printf("Error: Memory allocation failed for segment %d\n", id);
        exit(1);
    }
    segment->limit = limit;       // Set the segment's limit
    segment->id = id;            // Assign segment identifier
    
    // Initialize segment memory with zeros
    for (int i = 0; i < limit; i++) {
        segment->base[i] = 0;
    }
    printf("Initialized Segment %d with base at %p and limit %d\n", 
           id, (void *)segment->base, limit);
}

// Function to read data from a segment at a specific offset
int readFromSegment(Segment *segment, int offset) {
    // Check if offset is within bounds
    if (offset >= 0 && offset < segment->limit) {
        int data = segment->base[offset];
        printf("Read from Segment %d, offset %d: %d\n", segment->id, offset, data);
        return data;
    } else {
        // Handle out-of-bounds access
        printf("Error: Segment %d access violation at offset %d (limit: %d)\n", 
               segment->id, offset, segment->limit);
        return -1;
    }
}

// Function to write data to a segment at a specific offset
void writeToSegment(Segment *segment, int offset, int data) {
    // Check if offset is within bounds
    if (offset >= 0 && offset < segment->limit) {
        segment->base[offset] = data;
        printf("Wrote %d to Segment %d, offset %d\n", data, segment->id, offset);
    } else {
        // Handle out-of-bounds access
        printf("Error: Segment %d access violation at offset %d (limit: %d)\n", 
               segment->id, offset, segment->limit);
    }
}

// Function to free a segment's allocated memory
void freeSegment(Segment *segment) {
    if (segment->base != NULL) {  // Check if memory was allocated
        free(segment->base);      // Release the memory
        segment->base = NULL;     // Set pointer to NULL to prevent dangling reference
        printf("Freed Segment %d\n", segment->id);
    }
}

// Function to simulate memory access with segment selection
void accessMemory(Segment segments[], int segment_id, int offset, int data, bool write) {
    // Validate segment ID
    if (segment_id < 0 || segment_id >= NUM_SEGMENTS) {
        printf("Error: Invalid segment ID %d\n", segment_id);
        return;
    }
    
    if (write) {
        writeToSegment(&segments[segment_id], offset, data);
    } else {
        int read_data = readFromSegment(&segments[segment_id], offset);
        if (read_data != -1) {
            printf("Accessed data: %d\n", read_data);
        }
    }
}

// Function to print the current state of all segments
void printSegmentState(Segment segments[]) {
    printf("\nSegment State:\n");
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        printf("Segment %d: Base=%p, Limit=%d, Sample Data=[", 
               segments[i].id, (void *)segments[i].base, segments[i].limit);
        // Print first few elements
        for (int j = 0; j < (segments[i].limit < 5 ? segments[i].limit : 5); j++) {
            printf("%d", segments[i].base[j]);
            if (j < 4 && j < segments[i].limit - 1) printf(", ");
        }
        printf("]\n");
    }
}

int main() {
    Segment segments[NUM_SEGMENTS];  // Array of segments simulating memory regions
    int offset = 50;                // Sample offset for access
    
    // Initialize multiple segments with different limits
    initializeSegment(&segments[0], SEGMENT_SIZE, 0);      // Segment 0: 100 ints
    initializeSegment(&segments[1], SEGMENT_SIZE / 2, 1);  // Segment 1: 50 ints
    initializeSegment(&segments[2], SEGMENT_SIZE * 2, 2);  // Segment 2: 200 ints
    
    // Print initial segment state
    printSegmentState(segments);
    
    // Demonstrate memory operations
    printf("\nPerforming memory operations:\n");
    
    // Write to Segment 0
    accessMemory(segments, 0, offset, 42, true);
    
    // Read from Segment 0
    accessMemory(segments, 0, offset, 0, false);
    
    // Write to Segment 1 (within bounds)
    accessMemory(segments, 1, 25, 99, true);
    
    // Attempt out-of-bounds access on Segment 1
    accessMemory(segments, 1, 60, 123, true);
    
    // Write to Segment 2
    accessMemory(segments, 2, 150, 777, true);
    
    // Read from Segment 2
    accessMemory(segments, 2, 150, 0, false);
    
    // Clean up all segments
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        freeSegment(&segments[i]);
    }
    
    return 0;
}
