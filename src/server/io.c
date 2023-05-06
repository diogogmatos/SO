#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <glib.h>

#include "../../include/message.h" // struct MESSAGE
#include "../../include/status.h"  // struct STATUS
#include "../../include/global_utils.h"
#include "../../include/server/io.h"
#include "../../include/server/execute.h"

int send_status(GHashTable *log) {
    // open fifo
    int fd = open("response_fifo", O_WRONLY);
    if (fd == -1)
    {
        perror("open fifo response_fifo");
        return -1;
    }

    // iterate through hash table
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, log);
    while (g_hash_table_iter_next(&iter, &key, &value))
    {
        MESSAGE *m = value;

        // add program info to struct
        STATUS s = {0};
        s.pid = m->pid;
        strncpy(s.name, m->message, MESSAGE_SIZE);
        s.execution_time = get_execution_time(m->timestamp, get_timestamp_us());

        // write to fifo
        int w = write(fd, &s, sizeof(STATUS));
        if (w == -1)
        {
            perror("write status");
            return -1;
        }
    }

    // write end
    STATUS s = {0};
    s.pid = -1;

    int w = write(fd, &s, sizeof(STATUS));
    if (w == -1)
    {
        perror("write status END");
        return -1;
    }

    close(fd);

    return 0;
}

int handle_message(GHashTable *log, MESSAGE m, char *folder_path)
{
    switch (m.type)
    {
    case e_execute_u:
    {
        int r = execute_u(log, m, folder_path);
        if (r == -1)
        {
            perror("execute_u()");
            return -1;
        }
        return 0;
    }
    case e_execute_p:
    {
        printf("Not implemented!\n");
        return 0;
    }
    case e_status:
    {
        int r = send_status(log);
        if (r == -1)
        {
            perror("send_status()");
            return -1;
        }
        return 0;
    }
    case e_stats_time:
    {
        printf("Not implemented!\n");
        return 0;
    }
    case e_stats_command:
    {
        printf("Not implemented!\n");
        return 0;
    }
    case e_stats_uniq:
    {
        printf("Not implemented!\n");
        return 0;
    }
    default:
    {
        perror("Invalid message type");
        return -1;
    }
    }

    return 0;
}
