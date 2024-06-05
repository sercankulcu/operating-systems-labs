#include <stdio.h>
#include <stdlib.h>

// Define the number of quick lists
#define NUM_QUICK_LISTS 4

// Define the sizes of memory blocks in each quick list
#define SIZE_LIST_5   5
#define SIZE_LIST_10  10
#define SIZE_LIST_20  20
#define SIZE_LIST_40  40

// Define the structure of a memory block
typedef struct Block {
    size_t size;
    struct Block* next;
} Block;

// Array of pointers to the quick lists
Block* quick_lists[NUM_QUICK_LISTS] = {NULL};

// Memory pool
char memory_pool[(SIZE_LIST_5 + SIZE_LIST_10 + SIZE_LIST_20 + SIZE_LIST_40) * 10];

// Initialize memory pool
void init_memory_pool() {
    // Initialize quick list for 5 size blocks
    quick_lists[0] = (Block*)memory_pool;
    quick_lists[0]->size = SIZE_LIST_5;
    quick_lists[0]->next = NULL;

    // Initialize quick list for 10 size blocks
    quick_lists[1] = (Block*)(memory_pool + SIZE_LIST_5 * 10);
    quick_lists[1]->size = SIZE_LIST_10;
    quick_lists[1]->next = NULL;

    // Initialize quick list for 20 size blocks
    quick_lists[2] = (Block*)(memory_pool + (SIZE_LIST_5 + SIZE_LIST_10) * 10);
    quick_lists[2]->size = SIZE_LIST_20;
    quick_lists[2]->next = NULL;

    // Initialize quick list for 40 size blocks
    quick_lists[3] = (Block*)(memory_pool + (SIZE_LIST_5 + SIZE_LIST_10 + SIZE_LIST_20) * 10);
    quick_lists[3]->size = SIZE_LIST_40;
    quick_lists[3]->next = NULL;
}

// Quick fit malloc
void* quick_fit_malloc(size_t size) {
    Block* block = NULL;
    int index = 0;

    if(size <= SIZE_LIST_5) {
        index = 0;
    } else if(size <= SIZE_LIST_10) {
        index = 1;
    } else if(size <= SIZE_LIST_20) {
        index = 2;
    } else if(size <= SIZE_LIST_40) {
        index = 3;
    } else {
        // Unsupported size
        return NULL;
    }

    block = quick_lists[index];
    if (block == NULL) {
        printf("Memory (%d) allocation failed.\n", size);
        return NULL;
    }
    
    printf("Memory (%d) allocated.\n", size);
    quick_lists[index] = block->next;
    return (void*)(block + 1);
}

// Quick fit free
void quick_fit_free(void* ptr, size_t size) {
    if (ptr == NULL)
        return;

    Block* block = ((Block*)ptr) - 1;
    int index = 0;

    if(size <= SIZE_LIST_5) {
        index = 0;
    } else if(size <= SIZE_LIST_10) {
        index = 1;
    } else if(size <= SIZE_LIST_20) {
        index = 2;
    } else if(size <= SIZE_LIST_40) {
        index = 3;
    } else {
        // Unsupported size
        return;
    }

    block->next = quick_lists[index];
    quick_lists[index] = block;
    printf("Memory (%d) deallocated.\n", size);
}

// Display memory pool and allocated blocks
void display_memory() {
    printf("Memory Layout: ");

    for (int i = 0; i < NUM_QUICK_LISTS; i++) {
        printf("(%d,", (i == 0 ? SIZE_LIST_5 : (i == 1 ? SIZE_LIST_10 : (i == 2 ? SIZE_LIST_20 : SIZE_LIST_40))));
        Block* current_block = quick_lists[i];

        if (current_block == NULL) {
            printf(" used)");
        } else {
            while (current_block != NULL) {
                printf(" free)");
                current_block = current_block->next;
            }
        }
    }
    printf("\n");
}

int main() {
    // Initialize memory pool
    init_memory_pool();

    // Allocate memory using quick fit malloc
    display_memory();
    void* ptr1 = quick_fit_malloc(10);
    display_memory();
    void* ptr2 = quick_fit_malloc(20);
    display_memory();
    void* ptr3 = quick_fit_malloc(30);
    display_memory();
    void* ptr4 = quick_fit_malloc(40);
    display_memory();

    // Free allocated memory
    quick_fit_free(ptr1, SIZE_LIST_5);
    quick_fit_free(ptr2, SIZE_LIST_10);
    quick_fit_free(ptr3, SIZE_LIST_20);
    quick_fit_free(ptr4, SIZE_LIST_40);

    return 0;
}

