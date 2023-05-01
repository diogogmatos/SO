#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>

#include "../../include/client/execute.h"
#include "../../include/client/utils.h"
#include "../../include/message.h" // struct MESSAGE

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
        MESSAGE m_start = {0};
        m_start.pid = pid;
        m_start.type = e_execute;
        m_start.timestamp = clock();
        strncpy(m_start.message, argv[0], MESSAGE_SIZE);

        // write to fifo
        int w = write(fd, &m_start, sizeof(MESSAGE));
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
        MESSAGE m_end = {0};
        m_end.pid = r;
        m_end.type = e_close_info;
        m_end.timestamp = clock();
        sprintf(m_end.message, "%s END", argv[0]);

        // write to fifo
        w = write(fd, &m_end, sizeof(MESSAGE));
        if (w == -1)
        {
            perror("write end");
            return -1;
        }

        // execution time
        clock_t interval = m_end.timestamp - m_start.timestamp;
        printf("execution time: %g ms\n", (double) interval / CLOCKS_PER_SEC);

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
    MESSAGE m = {0};
    m.type = e_quit_server;
    
    // write to fifo
    int w = write(fd, &m, sizeof(MESSAGE));
    if (w == -1)
    {
        perror("write quit");
        return -1;
    }

    return 0;
}
