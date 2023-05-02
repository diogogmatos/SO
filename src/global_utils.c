#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../include/utils.h"

double get_execution_time(clock_t start, clock_t end)
{
    return (double) (end - start) / 10e3;
}
