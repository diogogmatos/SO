#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>

#include "../../include/client/utils.h"

// turns a string into an array of strings, separated by space
char **str_to_array(char *str)
{
    char **array = malloc(sizeof(char *) * BUFSIZ);
    char *token = strtok(str, " ");

    int i = 0;
    while (token != NULL)
    {
        array[i] = token;
        token = strtok(NULL, " ");
        i++;
    }

    return array;
}

double execution_time(long sec, long usec) {
    return sec * 1000 + (double) usec / 1000;
}
