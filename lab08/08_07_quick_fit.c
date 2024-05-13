#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE 100

// Structure to represent a memory block
typedef struct MemoryBlock {
    int size;             // Size of the block
    int isFree;           // Flag to indicate whether the block is free (1) or allocated (0)
    struct MemoryBlock *next;  // Pointer to the next block
} MemoryBlock;

MemoryBlock *memory = NULL;
MemoryBlock *freeList[2] = {NULL, NULL}; // Separate free lists for two block sizes

// Function to initialize memory with a single free block
void initializeMemory() {
    memory = (MemoryBlock *)malloc(sizeof(MemoryBlock));
    memory->size = MEMORY_SIZE;
    memory->isFree = 1;
    memory->next = NULL;
    // Add the entire memory as a free block to the free list
    freeList[0] = memory;
}

// Function to allocate memory using quick fit strategy
void *quickFitAllocate(int size) {
    int index = (size <= 50) ? 0 : 1; // Index for selecting the free list based on block size
    if (freeList[index] == NULL) {
        printf("Error: No suitable block available in the free list for size %d\n", size);
        return NULL;
    }
    MemoryBlock *allocatedBlock = freeList[index];
    freeList[index] = allocatedBlock->next;
    allocatedBlock->isFree = 0;
    return (void *)(allocatedBlock + 1); // Return pointer to the data area of the block
}

// Function to free allocated memory
void quickFitDeallocate(void *ptr) {
    if (ptr == NULL) return;
    MemoryBlock *block = (MemoryBlock *)ptr - 1;
    block->isFree = 1;
    int index = (block->size <= 50) ? 0 : 1; // Index for selecting the free list based on block size
    block->next = freeList[index];
    freeList[index] = block;
}

// Function to display memory blocks
void displayMemory() {
    MemoryBlock *current = memory;
    printf("Memory Layout:\n");
    while (current != NULL) {
        printf("Size: %d, Free: %s\n", current->size, current->isFree ? "Yes" : "No");
        current = current->next;
    }
}

int main() {
    initializeMemory();
    
    // Allocate memory using quick fit
    void *ptr1 = quickFitAllocate(20);
    void *ptr2 = quickFitAllocate(30);
    void *ptr3 = quickFitAllocate(60);

    // Display memory layout after allocation
    displayMemory();

    // Free memory
    quickFitDeallocate(ptr1);
    
    // Display memory layout after deallocation
    displayMemory();

    // Free memory
    quickFitDeallocate(ptr2);
    
    // Display memory layout after deallocation
    displayMemory();

    // Free memory
    quickFitDeallocate(ptr3);
    
    // Display memory layout after deallocation
    displayMemory();

    // Free memory allocated for memory block
    free(memory);

    return 0;
}

/* We maintain separate free lists for two block sizes: blocks of size 50 or less and blocks of size greater than 50.
The quickFitAllocate() function selects the appropriate free list based on the requested size and allocates a block from that list if available.
*/
