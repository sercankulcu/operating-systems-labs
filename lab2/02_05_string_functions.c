#include "02_05_string_functions.h"
#include <string.h>
#include <stdlib.h>

// Function to calculate the length of a string
int str_length(const char *str) {
    return strlen(str);
}

// Function to concatenate two strings
char *str_concat(const char *str1, const char *str2) {
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    char *result = (char *)malloc(len1 + len2 + 1); // +1 for null terminator

    if (result == NULL) {
        // Memory allocation failed
        return NULL;
    }

    strcpy(result, str1);
    strcat(result, str2);
    return result;
}

// Function to compare two strings
int str_compare(const char *str1, const char *str2) {
    return strcmp(str1, str2);
}

