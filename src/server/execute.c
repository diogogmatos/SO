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
#include "../../include/status.h" // struct STATUS
#include "../../include/global_utils.h"
#include "../../include/server/execute.h"

int execute_u(GHashTable *log, MESSAGE m, char *folder_path)
{
    // if prog is not in hash table, add it
    if (!g_hash_table_contains(log, &m.pid))
    {
        MESSAGE *h = malloc(sizeof(MESSAGE));
        h->pid = m.pid;
        h->type = m.type;
        h->timestamp = m.timestamp;
        strncpy(h->message, m.message, MESSAGE_SIZE);

        g_hash_table_insert(log, &h->pid, h);
    }
    // if prog is in hash table, write state to log file and remove it
    else
    {
        MESSAGE *ph = g_hash_table_lookup(log, &m.pid);

        STATUS p = {0};
        p.pid = ph->pid;
        strncpy(p.name, ph->message, MESSAGE_SIZE);
        p.execution_time = get_execution_time(ph->timestamp, m.timestamp);

        // create file path
        char file_path[strlen(folder_path) + 11];
        sprintf(file_path, "%s/%d", folder_path, m.pid);

        // create and open file
        int log_fd = open(file_path, O_CREAT | O_TRUNC | O_WRONLY, 0666);
        if (log_fd == -1)
        {
            perror("open log file");
            return -1;
        }

        // write to file
        write(log_fd, &p, sizeof(STATUS));

        // close file
        close(log_fd);

        // remove from hash table
        gboolean b = g_hash_table_remove(log, &m.pid);
        if (!b)
        {
            perror("remove from hash table");
            return -1;
        }

        // test file
        int test = open(file_path, O_RDONLY);

        STATUS pr;
        read(test, &pr, sizeof(STATUS));

        printf("added to log file:\n");
        printf("pid: %d\n", pr.pid);
        printf("name: %s\n", pr.name);
        printf("execution time: %g ms\n\n", pr.execution_time);

        close(test);
    }

    return 0;
}
