// Include necessary headers
#include <stdio.h>      // For standard input/output functions
#include <stdlib.h>     // For exit
#include <stdint.h>     // For intptr_t to handle pointer-integer conversions safely

// Define constants
#define MEMORY_SIZE 64      // Total memory size in bytes
#define BLOCK_SIZE 4        // Size of each memory block in bytes
#define NUM_BLOCKS (MEMORY_SIZE / BLOCK_SIZE)  // Number of blocks in memory

// Bitmap to track allocation status (1 = allocated, 0 = free)
unsigned char bitmap[NUM_BLOCKS];

// Simulated memory pool (for demonstration purposes)
char memory_pool[MEMORY_SIZE];

// Function to initialize the bitmap and memory pool
void initializeBitmap() {
    for (int i = 0; i < NUM_BLOCKS; i++) {
        bitmap[i] = 0;  // Mark all blocks as free
        memory_pool[i * BLOCK_SIZE] = 0;  // Initialize memory pool (first byte of each block)
    }
    printf("Bitmap and memory pool initialized: %d blocks of %d bytes each\n", 
           NUM_BLOCKS, BLOCK_SIZE);
}

// Function to allocate memory using bitmap
void *allocateMemory(int size) {
    int blocksNeeded = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;  // Ceiling division for required blocks
    int consecutiveFreeBlocks = 0;
    int startIndex = -1;
    
    // Search for consecutive free blocks
    for (int i = 0; i < NUM_BLOCKS; i++) {
        if (bitmap[i] == 0) {  // Block is free
            if (consecutiveFreeBlocks == 0) {
                startIndex = i;  // Potential start of free region
            }
            consecutiveFreeBlocks++;
            if (consecutiveFreeBlocks == blocksNeeded) {
                // Allocate the blocks
                for (int j = startIndex; j < startIndex + blocksNeeded; j++) {
                    bitmap[j] = 1;  // Mark as allocated
                }
                void *address = (void *)(&memory_pool[startIndex * BLOCK_SIZE]);
                printf("Allocated %d bytes (%d blocks) at %p\n", 
                       size, blocksNeeded, address);
                return address;
            }
        } else {
            consecutiveFreeBlocks = 0;  // Reset counter if block is allocated
        }
    }
    
    printf("Error: Not enough contiguous free memory for %d bytes\n", size);
    return NULL;
}

// Function to deallocate memory
void deallocateMemory(void *ptr, int size) {
    // Convert pointer to index safely using intptr_t
    intptr_t ptr_int = (intptr_t)ptr;
    intptr_t pool_base = (intptr_t)memory_pool;
    int startIndex = (ptr_int - pool_base) / BLOCK_SIZE;
    int blocksNeeded = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    
    // Validate pointer and range
    if (startIndex < 0 || startIndex >= NUM_BLOCKS || 
        startIndex + blocksNeeded > NUM_BLOCKS) {
        printf("Error: Invalid pointer %p or size %d for deallocation\n", ptr, size);
        return;
    }
    
    // Free the blocks
    for (int i = startIndex; i < startIndex + blocksNeeded; i++) {
        if (bitmap[i] == 1) {
            bitmap[i] = 0;  // Mark as free
        } else {
            printf("Warning: Block %d at %p was already free\n", 
                   i, &memory_pool[i * BLOCK_SIZE]);
        }
    }
    printf("Deallocated %d bytes (%d blocks) at %p\n", size, blocksNeeded, ptr);
}

// Function to display memory allocation status and sample data
void displayMemory() {
    printf("\nMemory Allocation Status:\n");
    printf("Block: ");
    for (int i = 0; i < NUM_BLOCKS; i++) {
        printf("%2d ", i);
    }
    printf("\nState: ");
    for (int i = 0; i < NUM_BLOCKS; i++) {
        printf("%2d ", bitmap[i]);
    }
    printf("\nData:  ");
    for (int i = 0; i < NUM_BLOCKS; i++) {
        printf("%2d ", (unsigned char)memory_pool[i * BLOCK_SIZE]);  // Show first byte
    }
    printf("\n");
}

// Function to write data to memory (for demonstration)
void writeToMemory(void *ptr, int size, char value) {
    intptr_t ptr_int = (intptr_t)ptr;
    intptr_t pool_base = (intptr_t)memory_pool;
    int startIndex = (ptr_int - pool_base) / BLOCK_SIZE;
    int blocksNeeded = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    
    if (startIndex < 0 || startIndex + blocksNeeded > NUM_BLOCKS) {
        printf("Error: Invalid write to %p\n", ptr);
        return;
    }
    
    for (int i = 0; i < size && (startIndex * BLOCK_SIZE + i) < MEMORY_SIZE; i++) {
        memory_pool[startIndex * BLOCK_SIZE + i] = value;
    }
    printf("Wrote %d bytes of value %d to %p\n", size, value, ptr);
}

int main() {
    initializeBitmap();
    displayMemory();
    
    // Allocate and use memory blocks
    printf("\nAllocating memory for block 1 (8 bytes):\n");
    void *ptr1 = allocateMemory(8);
    if (ptr1) writeToMemory(ptr1, 8, 42);
    displayMemory();
    
    printf("\nAllocating memory for block 2 (12 bytes):\n");
    void *ptr2 = allocateMemory(12);
    if (ptr2) writeToMemory(ptr2, 12, 99);
    displayMemory();
    
    printf("\nFreeing memory for block 1:\n");
    deallocateMemory(ptr1, 8);
    displayMemory();
    
    printf("\nAllocating memory for block 3 (16 bytes):\n");
    void *ptr3 = allocateMemory(16);
    if (ptr3) writeToMemory(ptr3, 16, 77);
    displayMemory();
    
    printf("\nFreeing memory for block 2:\n");
    deallocateMemory(ptr2, 12);
    displayMemory();
    
    printf("\nFreeing memory for block 3:\n");
    deallocateMemory(ptr3, 16);
    displayMemory();
    
    return 0;
}
