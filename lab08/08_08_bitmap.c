#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE 64 // Total memory size in bytes
#define BLOCK_SIZE 4   // Size of each memory block in bytes

// Bitmap to represent the allocation status of each block
unsigned char bitmap[MEMORY_SIZE / BLOCK_SIZE];

// Function to initialize the bitmap
void initializeBitmap() {
    for (int i = 0; i < MEMORY_SIZE / BLOCK_SIZE; i++) {
        bitmap[i] = 0; // Initialize all blocks as free
    }
}

// Function to allocate memory
void *allocateMemory(int size) {
    int blocksNeeded = (size + BLOCK_SIZE - 1) / BLOCK_SIZE; // Calculate number of blocks needed
    int consecutiveFreeBlocks = 0;
    int startIndex = -1;

    for (int i = 0; i < MEMORY_SIZE / BLOCK_SIZE; i++) {
        if (bitmap[i] == 0) { // Block is free
            if (consecutiveFreeBlocks == 0) {
                startIndex = i; // Start of a potential consecutive free block
            }
            consecutiveFreeBlocks++;
            if (consecutiveFreeBlocks == blocksNeeded) {
                // Allocate the consecutive free blocks
                for (int j = startIndex; j < startIndex + blocksNeeded; j++) {
                    bitmap[j] = 1;
                }
                return (void *)(startIndex * BLOCK_SIZE);
            }
        } else {
            consecutiveFreeBlocks = 0;
        }
    }

    printf("Error: Not enough free memory\n");
    return NULL;
}

// Function to free allocated memory
void deallocateMemory(void *ptr, int size) {
    int startIndex = (int)ptr / BLOCK_SIZE;
    int blocksNeeded = (size + BLOCK_SIZE - 1) / BLOCK_SIZE; // Calculate number of blocks needed

    for (int i = startIndex; i < startIndex + blocksNeeded; i++) {
        if (i < MEMORY_SIZE / BLOCK_SIZE) {
            bitmap[i] = 0; // Mark blocks as free
        }
    }
}

// Function to display memory allocation status
void displayMemory() {
    printf("Memory Allocation Status:\n");
    for (int i = 0; i < MEMORY_SIZE / BLOCK_SIZE; i++) {
        printf("%d ", bitmap[i]);
    }
    printf("\n");
}

int main() {
    initializeBitmap();

    // Allocate memory
    printf("Allocating memory for block 1...\n");
    void *ptr1 = allocateMemory(8); // Allocate memory for a block of 8 bytes
    displayMemory();

    // Allocate memory
    printf("Allocating memory for block 2...\n");
    void *ptr2 = allocateMemory(12); // Allocate memory for a block of 12 bytes
    displayMemory();

    // Free memory
    printf("Freeing memory for block 1...\n");
    deallocateMemory(ptr1, 8); // Free memory for block 1
    displayMemory();

    // Allocate memory
    printf("Allocating memory for block 3...\n");
    void *ptr3 = allocateMemory(16); // Allocate memory for a block of 16 bytes
    displayMemory();

    // Free memory
    printf("Freeing memory for block 2...\n");
    deallocateMemory(ptr2, 12); // Free memory for block 2
    displayMemory();

    // Free memory
    printf("Freeing memory for block 3...\n");
    deallocateMemory(ptr3, 16); // Free memory for block 3
    displayMemory();

    return 0;
}
/* We use a bitmap to represent the allocation status of each block of memory. Each bit in the bitmap corresponds to one memory block.
 * */
