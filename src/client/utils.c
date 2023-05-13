#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include "../../include/client/utils.h"

#define ARRAY_SIZE 1024

// turns a string into an array of strings, separated by delim
char **str_to_array(char *string, char *delim, int *size)
{
    char *str = strdup(string);
    char **array = malloc(sizeof(char *) * ARRAY_SIZE);
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

// remove spaces from the beginning and end of a string
void remove_leading_trailing_spaces(char *str)
{
    size_t len = strlen(str);
    if (len == 0)
    {
        return;
    }

    // Remove leading spaces
    size_t i = 0;
    while (isspace(str[i]))
    {
        i++;
    }
    memmove(str, str + i, len - i + 1);

    // Remove trailing spaces
    len = strlen(str);
    i = len - 1;
    while (isspace(str[i]))
    {
        i--;
    }
    str[i + 1] = '\0';
}
