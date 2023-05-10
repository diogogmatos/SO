#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>

#include "../../include/client/utils.h"

#define ARRAY_SIZE 1024

// turns a string into an array of strings, separated by space
char **str_to_array(char *string, char *delim, int *size)
{
    char *str = strdup(string);

    char **array = malloc(sizeof(char*) * ARRAY_SIZE);

    int i;
    for (i = 0; str != NULL; i++)
        array[i] = strsep(&str, delim);

    array[i] = NULL;

    *size = i;

    return array;
}

// turns an array of strings into an array of ints
int *str_array_to_int_array(char **array, int size)
{
    int *int_array = malloc(sizeof(int) * size);

    for (int i = 0; i < size; i++)
        int_array[i] = atoi(array[i]);

    return int_array;
}
