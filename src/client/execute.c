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
#include "../../include/global_utils.h"
#include "../../include/client/utils.h"
#include "../../include/message.h" // struct MESSAGE

int execute_u(char *args)
{
    int size;
    char **argv = str_to_array(args, " ", &size);

    int pid;

    clock_t start = get_timestamp_us();
    
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
        // write pid to stdout
        printf("pid: %d\n", pid);
        
        // open fifo
        int fd = open("fifo", O_WRONLY);
        if (fd == -1)
        {
            perror("open fifo");
            return -1;
        }

        // add program info to struct
        MESSAGE m_start = {0};
        m_start.pid = pid;
        m_start.type = e_execute_u;
        m_start.timestamp = start;
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
        m_end.type = e_execute_u;
        m_end.timestamp = get_timestamp_us();
        sprintf(m_end.message, "%s END", argv[0]);

        // write to fifo
        w = write(fd, &m_end, sizeof(MESSAGE));
        if (w == -1)
        {
            perror("write end");
            return -1;
        }

        // execution time
        printf("execution time: %g ms\n", get_execution_time(m_start.timestamp, m_end.timestamp));

        close(fd);
    }

    free(argv);

    return 0;
}

int function (char *args) 
{
    int size;    
    char **argv = str_to_array(args, " ", &size);

    int i;
    for (i = 0; argv[i] != NULL && strcmp("|", argv[i]); i++);

    if (!strcmp("|", argv[i]))
    {
        argv[i] = NULL;

        int fildes[2];

        int status_fildes = pipe(fildes);

        if (status_fildes == -1) 
        {
            perror("pipe");
            return -1;
        }

        close(fildes[0]);

        int status_dup = dup2(fildes[1], 1);

        if (status_dup == -1) 
        {
            close(fildes[1]);
            perror("dup");
            return -1;
        }

        int pid;

        // child
        if ((pid = fork()) == 0)
        {
            function(args+i+1);

            close(fildes[1]);

            exit(0);
        }
        // parent
        else
        {
            int exec_res = execvp(argv[0], argv);

            if (exec_res == -1)
            {
                perror("execvp");
            }

            exit(0);
        }

        close(fildes[1]);

    }
    else 
    {
        int exec_res = execvp(argv[0], argv);

        if (exec_res == -1)
        {
            perror("execvp");
        }

        exit(0);
    }

    return 0;
}

int execute_p(char *args)
{
    int size;
    char **argv = str_to_array(args, " ", &size);

    int pid;
    
    // child
    if ((pid = fork()) == 0)
    {
        function(args);

        exit(0);
    }
    // parent
    else
    {
        // write pid to stdout
        printf("pid: %d\n", pid);
        
        // open fifo
        int fd = open("fifo", O_WRONLY);
        if (fd == -1)
        {
            perror("open fifo");
            return -1;
        }

        // add program info to struct
        MESSAGE m_start = {0};
        m_start.pid = pid;
        m_start.type = e_execute_p;
        m_start.timestamp = get_timestamp_us();
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
        m_end.type = e_execute_p;
        m_end.timestamp = get_timestamp_us();
        sprintf(m_end.message, "%s END", argv[0]);

        // write to fifo
        w = write(fd, &m_end, sizeof(MESSAGE));
        if (w == -1)
        {
            perror("write end");
            return -1;
        }

        // execution time
        printf("execution time: %g ms\n", get_execution_time(m_start.timestamp, m_end.timestamp));

        close(fd);
    }

    free(argv);

    return 0;
}
