#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>

#include "../../include/client/execute.h"
#include "../../include/client/utils.h"
#include "../../include/prog_info.h" // struct PROG

int execute_u(char *args)
{
    char **argv = str_to_array(args);

    int pid;
    
    // child
    if ((pid = fork()) == 0)
    {
        int exec_res = execvp(argv[0], argv);

        if (exec_res == -1)
        {
            perror("execvp");
            return -1;
        }

        exit(0);
    }
    // parent
    else
    {
        // open fifo
        int fd = open("fifo", O_WRONLY);
        if (fd == -1)
        {
            perror("open fifo");
            return -1;
        }

        // write pid to stdout
        printf("pid: %d\n", pid);

        // add program info to struct
        PROG p_start;
        p_start.pid = pid;
        strcpy(p_start.name, argv[0]);
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        p_start.sec = tv_start.tv_sec;
        p_start.usec = tv_start.tv_usec;

        // write to fifo
        int w = write(fd, &p_start, sizeof(PROG));
        if (w == -1)
        {
            perror("write start");
            return -1;
        }

        // wait for child to finish
        int status;
        int r = wait(&status);
        if (r == -1)
        {
            perror("wait");
            return -1;
        }

        // add program close info to struct
        PROG p_end;
        p_end.pid = r;
        strcpy(p_end.name, "exit");
        struct timeval tv_end;
        gettimeofday(&tv_end, NULL);
        p_end.sec = tv_end.tv_sec;
        p_end.usec = tv_end.tv_usec;

        // write to fifo
        w = write(fd, &p_end, sizeof(PROG));
        if (w == -1)
        {
            perror("write end");
            return -1;
        }

        // execution time
        long sec = p_end.sec - p_start.sec;
        long usec = p_end.usec - p_start.usec;
        printf("Execution time: %lf ms\n", execution_time(sec, usec));

        close(fd);
    }

    free(argv);

    return 0;
}

int execute_p(char *args)
{
    return 0;
}

int quit_server()
{
    // open fifo
    int fd = open("fifo", O_WRONLY);
    if (fd == -1)
    {
        perror("open fifo");
        return -1;
    }

    // add program info to struct
    PROG p;
    p.pid = -1;
    strcpy(p.name, "quit");
    struct timeval tv;
    gettimeofday(&tv, NULL);
    p.sec = tv.tv_sec;
    p.usec = tv.tv_usec;

    // write to fifo
    int w = write(fd, &p, sizeof(PROG));
    if (w == -1)
    {
        perror("write quit");
        return -1;
    }

    return 0;
}
