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
#include "../../include/global_utils.h"

typedef struct prog_info
{
    int pid;
    char name[MESSAGE_SIZE];
    double execution_time;
} PROG;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        perror("Invalid number of arguments");
        return -1;
    }

    // folder where terminated programs state will be stored
    char *folder_path = argv[1];

    // create fifo
    int r = mkfifo("fifo", 0666);
    if (r == -1)
    {
        perror("mkfifo");
        return -1;
    }

    // open fifo
    int fd = open("fifo", O_RDONLY);
    if (fd == -1)
    {
        perror("open fifo");
        return -1;
    }

    // allows server to keep reading from fifo
    int keep_open = open("fifo", O_WRONLY);

    // create hash table to store program data, with pid as hash key
    GHashTable *log = g_hash_table_new(g_int_hash, g_int_equal);

    // read from fifo
    MESSAGE m;
    int bytes_read;
    while ((bytes_read = read(fd, &m, sizeof(MESSAGE))) > 0 && m.type != e_quit_server)
    {
        printf("pid: %d\n", m.pid);
        printf("type: %d\n", m.type);
        printf("timestamp: %ld\n", m.timestamp);
        printf("message: %s\n", m.message);
        printf("\n");
        
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

            PROG p = {0};
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
            write(log_fd, &p, sizeof(PROG));

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

            PROG pr;
            read(test, &pr, sizeof(PROG));

            printf("added to log file:\n");
            printf("pid: %d\n", pr.pid);
            printf("name: %s\n", pr.name);
            printf("execution time: %g ms\n\n", pr.execution_time);

            close(test);
        }
    }

    // close and unlink fifo
    close(fd);
    close(keep_open);
    unlink("fifo");
    unlink("keep_open");

    // free hash table
    g_hash_table_destroy(log);

    return 0;
}
