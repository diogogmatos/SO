#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <glib.h>

#include "../../include/client/stats.h"

#include "../../include/client/utils.h"
#include "../../include/message.h"
#include "../../include/stats_struct.h"

int stats_time(int argc, char **args)
{
    // open fifo
    int fd = open("fifo", O_WRONLY);
    if (fd == -1)
    {
        perror("open fifo");
        return -1;
    }

    // create message struct
    MESSAGE m = {0};
    m.type = e_stats_time;
    strncpy(m.message, "stats-uniq", 14);

    // write to fifo
    int w = write(fd, &m, sizeof(MESSAGE));
    if (w == -1)
    {
        perror("write stats-uniq message");
        return -1;
    }

    // create stats struct
    STATS s = {0};
    s.argc = argc;

    // copy pids to stats struct
    for (int i = 0; i < argc && i < SIZE; i++)
        s.pids[i] = atoi(args[i]);

    // write to fifo
    w = write(fd, &s, sizeof(STATS));
    if (w == -1)
    {
        perror("write stats-uniq args");
        return -1;
    }

    // make response fifo
    int r = mkfifo("response_fifo", 0666);
    if (r == -1)
    {
        perror("mkfifo response_fifo");
        return -1;
    }

    // open response fifo
    int fd_response = open("response_fifo", O_RDONLY);
    if (fd_response == -1)
    {
        perror("open response fifo");
        return -1;
    }

    // read from response fifo
    STATS st;
    int bytes_read;
    bytes_read = read(fd_response, &st, sizeof(STATS));
    if (bytes_read == -1)
    {
        perror("read response fifo");
        return -1;
    }

    // close response fifo
    close(fd_response);

    // print stats
    printf("total execution time: %g ms\n", st.total_time);

    // unlink response fifo
    unlink("response_fifo");

    return 0;
}

int stats_command(int argc, char **args, char *command)
{
    // open fifo
    int fd = open("fifo", O_WRONLY);
    if (fd == -1)
    {
        perror("open fifo");
        return -1;
    }

    // create message struct
    MESSAGE m = {0};
    m.type = e_stats_command;
    strncpy(m.message, "stats-uniq", 14);

    // write to fifo
    int w = write(fd, &m, sizeof(MESSAGE));
    if (w == -1)
    {
        perror("write stats-uniq message");
        return -1;
    }

    // create stats struct
    STATS s = {0};
    s.argc = argc;
    strncpy(s.command, command, strlen(command) + 1);

    // copy pids to stats struct
    for (int i = 0; i < argc && i < SIZE; i++)
        s.pids[i] = atoi(args[i]);

    // write to fifo
    w = write(fd, &s, sizeof(STATS));
    if (w == -1)
    {
        perror("write stats-uniq stats struct");
        return -1;
    }

    // make response fifo
    int r = mkfifo("response_fifo", 0666);
    if (r == -1)
    {
        perror("mkfifo response_fifo");
        return -1;
    }

    // open response fifo
    int fd_response = open("response_fifo", O_RDONLY);
    if (fd_response == -1)
    {
        perror("open response fifo");
        return -1;
    }

    // read from response fifo
    STATS st;
    int bytes_read;
    bytes_read = read(fd_response, &st, sizeof(STATS));
    if (bytes_read == -1)
    {
        perror("read response fifo");
        return -1;
    }

    // close response fifo
    close(fd_response);

    // print stats
    printf("executed %d time", st.total_processes);
    if (st.total_processes != 1)
        printf("s\n");
    else    
        printf("\n");

    // unlink response fifo
    unlink("response_fifo");

    return 0;
}

int stats_uniq(int argc, char **args)
{
    // open fifo
    int fd = open("fifo", O_WRONLY);
    if (fd == -1)
    {
        perror("open fifo");
        return -1;
    }

    // create message struct
    MESSAGE m = {0};
    m.type = e_stats_uniq;
    strncpy(m.message, "stats-uniq", 14);

    // write to fifo
    int w = write(fd, &m, sizeof(MESSAGE));
    if (w == -1)
    {
        perror("write stats-uniq message");
        return -1;
    }

    // create stats struct
    STATS s = {0};
    s.argc = argc;

    // copy pids to stats struct
    for (int i = 0; i < argc && i < SIZE; i++)
        s.pids[i] = atoi(args[i]);

    // make response fifo
    int r = mkfifo("response_fifo", 0666);
    if (r == -1)
    {
        perror("mkfifo response_fifo");
        return -1;
    }

    // write to fifo
    w = write(fd, &s, sizeof(STATS));
    if (w == -1)
    {
        perror("write stats-uniq stats struct");
        return -1;
    }

    // open response fifo
    int fd_response = open("response_fifo", O_RDONLY);
    if (fd_response == -1)
    {
        perror("open response fifo");
        return -1;
    }

    // read from response fifo
    STATS st;
    int bytes_read;
    while ((bytes_read = read(fd_response, &st, sizeof(STATS))) > 0 && st.argc > 0)
    {
        // print stats
        printf("%s\n", st.command);
    }

    // close and unlink response fifo
    close(fd_response);
    unlink("response_fifo");

    return 0;
}
