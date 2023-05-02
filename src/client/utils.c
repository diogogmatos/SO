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
char **str_to_array(char *string)
{
    char *str = strdup(string);

    char **array = malloc(sizeof(char*) * ARRAY_SIZE);

    int i;
    for (i = 0; str != NULL; i++)
        array[i] = strsep(&str, " ");

    array[i] = NULL;

    return array;
}

// function to get a timestamp in microseconds
clock_t get_timestamp_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    clock_t timestamp_us = tv.tv_sec * 10e6 + tv.tv_usec;
    return timestamp_us;
}
