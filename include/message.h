#ifndef message_h
#define message_h

#include <time.h>

#define MESSAGE_SIZE 240

typedef enum _message_type {
    e_execute,
    e_status,
    e_stats_time,
    e_stats_command,
    e_stats_uniq,
    e_close_info,
    e_quit_server
} MESSAGE_TYPE;

typedef struct _mt_fifo {
    int pid;           // 4 bytes
    MESSAGE_TYPE type; // 4 bytes
    clock_t timestamp; // 8 bytes
    char message[MESSAGE_SIZE]; // SIZE bytes
} MESSAGE;

#endif
