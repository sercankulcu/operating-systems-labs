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

// Function to initialize memory with a single free block
void initializeMemory() {
    memory = (MemoryBlock *)malloc(sizeof(MemoryBlock));
    memory->size = MEMORY_SIZE;
    memory->isFree = 1;
    memory->next = NULL;
}

// Function to allocate memory using worst fit strategy
void *worstFitAllocate(int size) {
    MemoryBlock *current = memory;
    MemoryBlock *worstFit = NULL;
    int maxFragmentation = -1; // Initialize with a value smaller than any possible fragmentation

    while (current != NULL) {
        if (current->isFree && current->size >= size) {
            int fragmentation = current->size - size;
            if (fragmentation > maxFragmentation) {
                maxFragmentation = fragmentation;
                worstFit = current;
            }
        }
        current = current->next;
    }

    if (worstFit != NULL) {
        // Allocate memory
        if (worstFit->size > size) {
            // Split the block if it's larger than requested
            MemoryBlock *newBlock = (MemoryBlock *)malloc(sizeof(MemoryBlock));
            newBlock->size = worstFit->size - size;
            newBlock->isFree = 1;
            newBlock->next = worstFit->next;
            worstFit->next = newBlock;
            worstFit->size = size;
        }
        worstFit->isFree = 0;
        return (void *)(worstFit + 1); // Return pointer to the data area of the block
    }

    return NULL; // Memory allocation failed
}

// Function to free allocated memory
void deallocate(void *ptr) {
    if (ptr == NULL) return;
    MemoryBlock *block = (MemoryBlock *)ptr - 1;
    block->isFree = 1;
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
    
    // Allocate memory using worst fit
    void *ptr1 = worstFitAllocate(20);
    void *ptr2 = worstFitAllocate(30);

    // Display memory layout after allocation
    displayMemory();

    // Free memory
    deallocate(ptr1);
    
    // Display memory layout after deallocation
    displayMemory();

    // Free memory
    deallocate(ptr2);
    
    // Display memory layout after deallocation
    displayMemory();

    // Free memory allocated for memory block
    free(memory);

    return 0;
}

/* We've implemented the worstFitAllocate() function to search for the worst-fit free block that maximizes internal fragmentation.
It iterates through the memory blocks, calculates the fragmentation for each free block, and selects the block with the maximum fragmentation that is large enough to hold the requested size.
*/
