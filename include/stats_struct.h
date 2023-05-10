#ifndef stats_struct_h
#define stats_struct_h

#define SIZE 50

typedef struct stats {
    int argc; // nr of args
    int pids[SIZE];
    char command[SIZE];
    double total_time; // stats-time
    int total_processes; // stats-command
} STATS;

#endif
