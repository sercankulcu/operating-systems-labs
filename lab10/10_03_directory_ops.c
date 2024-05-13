#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main() {
    const char *dir_name = "example_directory";

    // Create a directory
    if (mkdir(dir_name, 0777) == -1) {
        perror("mkdir");
        exit(EXIT_FAILURE);
    } else {
        printf("Directory '%s' created successfully.\n", dir_name);
    }

    // Change directory
    if (chdir(dir_name) == -1) {
        perror("chdir");
        exit(EXIT_FAILURE);
    } else {
        printf("Changed to directory '%s'.\n", dir_name);
    }

    // List directory contents
    printf("Contents of directory '%s':\n", dir_name);
    system("ls -l");

    // Change back to the parent directory
    if (chdir("..") == -1) {
        perror("chdir");
        exit(EXIT_FAILURE);
    } else {
        printf("Changed back to parent directory.\n");
    }

    // Remove directory
    if (rmdir(dir_name) == -1) {
        perror("rmdir");
        exit(EXIT_FAILURE);
    } else {
        printf("Directory '%s' removed successfully.\n", dir_name);
    }

    return 0;
}

