#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Executing an external executable file...\n");

    // Execute an external executable file
    int result = system("./external_executable");

    if (result == 0) {
        printf("External executable file executed successfully.\n");
    } else {
        printf("Failed to execute external executable file.\n");
    }

    return 0;
}

