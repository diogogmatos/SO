#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <sys/time.h>

#include "../include/global_utils.h"

double get_execution_time(clock_t start, clock_t end)
{
    return (double) (end - start) / 10e3;
}

// function to get a timestamp in microseconds
clock_t get_timestamp_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    clock_t timestamp_us = tv.tv_sec * 10e6 + tv.tv_usec;
    return timestamp_us;
}
