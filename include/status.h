#ifndef status_h
#define status_h

#define MESSAGE_SIZE 240

typedef struct status {
    int pid;
    char name[MESSAGE_SIZE];
    double execution_time;
} STATUS;

#endif
