#include <stdio.h>
#include <stdlib.h>
#include "02_05_string_functions.h"

int main() {
    const char *str1 = "Hello, ";
    const char *str2 = "world!";
    char *concatenated;
    int length, comparison;

    // Using string functions
    length = str_length(str1);
    printf("Length of \"%s\": %d\n", str1, length);

    concatenated = str_concat(str1, str2);
    if (concatenated != NULL) {
        printf("Concatenated string: %s\n", concatenated);
        free(concatenated);
    } else {
        printf("Memory allocation failed for concatenated string.\n");
    }

    comparison = str_compare(str1, str2);
    if (comparison < 0) {
        printf("\"%s\" is lexicographically less than \"%s\"\n", str1, str2);
    } else if (comparison == 0) {
        printf("\"%s\" is equal to \"%s\"\n", str1, str2);
    } else {
        printf("\"%s\" is lexicographically greater than \"%s\"\n", str1, str2);
    }

    return 0;
}

