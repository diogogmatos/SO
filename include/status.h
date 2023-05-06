#ifndef status_h
#define status_h

#include <time.h>

typedef struct status {
    int pid;
    char name[MESSAGE_SIZE];
    double execution_time;
} STATUS;

#endif
