#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <glib.h>

#include "../../include/server/stats.h"

#include "../../include/stats_struct.h"
#include "../../include/status.h"

int stats_time(char *folder_path)
{
    // open fifo
    int fd = open("fifo", O_RDONLY);
    if (fd == -1)
    {
        perror("open fifo");
        return -1;
    }

    // read from fifo
    STATS s;
    int bytes_read;
    bytes_read = read(fd, &s, sizeof(STATS));
    if (bytes_read == -1)
    {
        perror("read stats-time");
        return -1;
    }

    // close fifo
    close(fd);

    // create pipe
    int fds[2];
    pipe(fds);

    for (int i=0; i < s.argc; i+=3) // 3 pids per child
    {
        int pid;
        if ((pid = fork()) == 0)
        {
            // close read end
            close(fds[0]);

            double time = 0;

            // read file
            for (int j = i; j < i+3 && j < s.argc; j++)
            {
                // create file path
                char file_path[strlen(folder_path) + 11];
                sprintf(file_path, "%s/%d", folder_path, s.pids[j]);

                // open file
                int log_fd = open(file_path, O_RDONLY);
                if (log_fd == -1)
                {
                    perror("open log file");
                    return -1;
                }

                STATUS p;
                int bytes_read;
                bytes_read = read(log_fd, &p, sizeof(STATUS));
                if (bytes_read == -1)
                {
                    perror("read log file");
                    return -1;
                }

                time += p.execution_time;

                // close file
                close(log_fd);
            }

            // write to pipe
            int w = write(fds[1], &time, sizeof(double));
            if (w == -1)
            {
                perror("write stats-time to pipe");
                return -1;
            }

            // exit child
            exit(0);
        }
    }

    // close write end
    close(fds[1]);

    double total_time = 0;

    for (int i=0; i < (s.argc / 3) + (s.argc % 3); i++)
    {
        double time;

        int r = read(fds[0], &time, sizeof(double));
        if (r == -1)
        {
            perror("read from pipe");
            return -1;
        }

        total_time += time;
    }

    // close read end
    close(fds[0]);

    printf("total execution time: %g ms\n", total_time);

    // open response fifo
    int fd_response = open("response_fifo", O_WRONLY);
    if (fd_response == -1)
    {
        perror("open response fifo");
        return -1;
    }

    // create response struct
    STATS s_response = {0};
    s_response.total_time = total_time;

    // write to response fifo
    int w = write(fd_response, &s_response, sizeof(STATS));
    if (w == -1)
    {
        perror("write total time to response fifo");
        return -1;
    }

    // close response fifo
    close(fd_response);

    return 0;
}

int stats_command(char *folder_path)
{
    // open fifo
    int fd = open("fifo", O_RDONLY);
    if (fd == -1)
    {
        perror("open fifo");
        return -1;
    }

    // read from fifo
    STATS s;
    int bytes_read;
    bytes_read = read(fd, &s, sizeof(STATS));
    if (bytes_read == -1)
    {
        perror("read stats-command");
        return -1;
    }

    // close fifo
    close(fd);

    // create pipe
    int fds[2];
    pipe(fds);

    for (int i=0; i < s.argc; i+=3) // 3 pids per child
    {
        int pid;
        if ((pid = fork()) == 0)
        {
            // close read end
            close(fds[0]);

            int count = 0;

            // read file
            for (int j = i; j < i+3 && j < s.argc; j++)
            {
                // create file path
                char file_path[strlen(folder_path) + 11];
                sprintf(file_path, "%s/%d", folder_path, s.pids[j]);

                // open file
                int log_fd = open(file_path, O_RDONLY);
                if (log_fd == -1)
                {
                    perror("open log file");
                    return -1;
                }

                STATUS p;
                int bytes_read;
                bytes_read = read(log_fd, &p, sizeof(STATUS));
                if (bytes_read == -1)
                {
                    perror("read log file");
                    return -1;
                }

                if (!strcmp(p.name, s.command))
                    count++;

                // close file
                close(log_fd);
            }

            // write to pipe
            int w = write(fds[1], &count, sizeof(int));
            if (w == -1)
            {
                perror("write stats-time to pipe");
                return -1;
            }

            // exit child
            exit(0);
        }
    }

    // close write end
    close(fds[1]);

    int total = 0;

    for (int i=0; i < (s.argc / 3) + (s.argc % 3); i++)
    {
        int count;

        int r = read(fds[0], &count, sizeof(int));
        if (r == -1)
        {
            perror("read from pipe");
            return -1;
        }

        total += count;
    }

    // close read end
    close(fds[0]);

    // open response fifo
    int fd_response = open("response_fifo", O_WRONLY);
    if (fd_response == -1)
    {
        perror("open response fifo");
        return -1;
    }

    // create response struct
    STATS s_response = {0};
    s_response.total_processes = total;

    // write to response fifo
    int w = write(fd_response, &s_response, sizeof(STATS));
    if (w == -1)
    {
        perror("write total time to response fifo");
        return -1;
    }

    // close response fifo
    close(fd_response);

    return 0;
}

int stats_uniq(char *folder_path)
{
    // open fifo
    int fd = open("fifo", O_RDONLY);
    if (fd == -1)
    {
        perror("open fifo");
        return -1;
    }

    // read from fifo
    STATS s;
    int bytes_read;
    bytes_read = read(fd, &s, sizeof(STATS));
    if (bytes_read == -1)
    {
        perror("read stats-command");
        return -1;
    }

    // close fifo
    close(fd);

    // create pipe
    int fds[2];
    pipe(fds);

    for (int i=0; i < s.argc; i+=3) // 3 pids per child
    {
        int pid;
        if ((pid = fork()) == 0)
        {
            // close read end
            close(fds[0]);

            // read file
            for (int j = i; j < i+3 && j < s.argc; j++)
            {
                // create file path
                char file_path[strlen(folder_path) + 11];
                sprintf(file_path, "%s/%d", folder_path, s.pids[j]);

                // open file
                int log_fd = open(file_path, O_RDONLY);
                if (log_fd == -1)
                {
                    perror("open log file");
                    return -1;
                }

                STATUS p;
                int bytes_read;
                bytes_read = read(log_fd, &p, sizeof(STATUS));
                if (bytes_read == -1)
                {
                    perror("read log file");
                    return -1;
                }

                // close file
                close(log_fd);

                // write prog name to pipe
                int w = write(fds[1], &p.name, MESSAGE_SIZE);
                if (w == -1)
                {
                    perror("write prog name to pipe");
                    return -1;
                }
            }

            // exit child
            exit(0);
        }
    }

    // close write end
    close(fds[1]);

    // create hash table to log names
    GHashTable *names = g_hash_table_new(g_str_hash, g_str_equal);

    // read from pipe
    for (int i=0; i < s.argc; i++)
    {
        char name[MESSAGE_SIZE];
        int r = read(fds[0], &name, sizeof(char) * MESSAGE_SIZE);
        if (r == -1)
        {
            perror("read prog name from pipe");
            return -1;
        }

        // if name is not in hash table, add it
        if (!g_hash_table_contains(names, name))
        {
            char *h = malloc(sizeof(char) * MESSAGE_SIZE);
            strncpy(h, name, MESSAGE_SIZE);

            g_hash_table_insert(names, h, NULL);
        }
    }

    // close read end
    close(fds[0]);

    // open response fifo
    int fd_response = open("response_fifo", O_WRONLY);
    if (fd_response == -1)
    {
        perror("open response fifo");
        return -1;
    }

    // response
    // iterate through hash table
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, names);
    while (g_hash_table_iter_next(&iter, &key, &value))
    {
        char *name = key;

        // add program info to struct
        STATS s = {0};
        s.argc = 1;
        strncpy(s.command, name, strlen(name) + 1);

        // write to fifo
        int w = write(fd, &s, sizeof(STATUS));
        if (w == -1)
        {
            perror("write prog name to response fifo");
            return -1;
        }
    }

    // write end
    STATS se = {0};
    se.argc = 0;

    int w = write(fd, &se, sizeof(STATUS));
    if (w == -1)
    {
        perror("write end message to response fifo");
        return -1;
    }

    // close response fifo
    close(fd_response);

    // destroy hash table
    g_hash_table_destroy(names);

    return 0;
}
