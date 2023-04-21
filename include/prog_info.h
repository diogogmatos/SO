#ifndef prog_info_h
#define prog_info_h

#include <sys/time.h>

typedef struct prog_info
{
    int pid;
    char name[256];
    time_t sec;
    suseconds_t usec;
} PROG;

#endif