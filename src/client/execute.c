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
        sprintf(m_end.message, "%s - END", argv[0]);

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

int execute_p(char *args)
{
    int size;
    printf("%s\n", args);
    char **progs = str_to_array(args, "|", &size);
    for (int i=0; i < size; i++)
        remove_leading_trailing_spaces(progs[i]);

    // create file descriptors
    int fds[size][2];

    // create pipes
    for (int i=0; i < size; i++)
        pipe(fds[i]);

    // create array to store pids
    int pids[size];

    // memorize stdin and stdout
    int input = dup(0);
    int output = dup(1);

    clock_t start = get_timestamp_us();

    for (int i=0; i < size; i++)
    {
        int pid;
        int argc;
        char **args = str_to_array(progs[i], " ", &argc);

        printf("%s\n", args[0]);

        if ((pid = fork()) == 0)
        {
            if (i == 0) // first program
            {
                // redirect STDOUT to pipe
                dup2(fds[i][1], output);

                // close unused read end
                close(fds[i][0]);
            }
            else if (i == size-1) // final program
            {
                // redirect STDIN to pipe
                dup2(fds[i-1][0], input);

                // close unused write end
                close(fds[i-1][1]);

                dup2(output, 1);
            }
            else // middle program
            {
                // redirect STDIN to pipe
                dup2(fds[i-1][0], input);

                // close unused write end
                close(fds[i-1][1]);

                // redirect STDOUT to pipe
                dup2(fds[i][1], output);

                // close unused read end
                close(fds[i][0]);
            }

            // execute
            int exec_res = execvp(args[0], args);
            if (exec_res == -1)
            {
                perror("execvp");
                return -1;
            }

            exit(0);
        }
        else
        {
            // store pid
            pids[i] = pid;
        }

        // free args
        free(args);
    }

    // close all pipe file descriptors in the parent process
    for (int i = 0; i < size; i++)
    {
        close(fds[i][0]);
        close(fds[i][1]);
    }

    // restore stdin and stdout
    dup2(input, 0);
    dup2(output, 1);
    close(input);
    close(output);

    // write pid to stdout
    printf("pid: %d\n", pids[0]);

    // add program start info to struct
    MESSAGE m_start = {0};
    m_start.pid = pids[0];
    m_start.type = e_execute_p;
    m_start.timestamp = start;

    char *message = malloc(MESSAGE_SIZE * sizeof(char));
    for (int i=0; i < size; i++)
    {
        int argc;
        char **args = str_to_array(progs[i], " ", &argc);

        if (i > 0) strcat(message, " | ");

        message = strcat(message, args[0]);

        free(args);
    }

    strncpy(m_start.message, message, MESSAGE_SIZE);

    // open fifo
    int fd = open("fifo", O_WRONLY);
    if (fd == -1)
    {
        perror("open fifo");
        return -1;
    }

    // write start to fifo
    int w = write(fd, &m_start, sizeof(MESSAGE));
    if (w == -1)
    {
        perror("write start");
        return -1;
    }

    // wait for children to finish
    for (int i=0; i < size; i++)
    {
        int status;
        int r = waitpid(pids[i], &status, 0);
        if (r == -1)
        {
            perror("waitpid");
            return -1;
        }
    }

    // add program end info to struct
    MESSAGE m_end = {0};
    m_end.pid = m_start.pid;
    m_end.type = e_execute_p;
    m_end.timestamp = get_timestamp_us();
    sprintf(m_end.message, "%s - END", message);

    // free message
    free(message);

    // write end to fifo
    w = write(fd, &m_end, sizeof(MESSAGE));
    if (w == -1)
    {
        perror("write end");
        return -1;
    }

    // close fifo
    close(fd);

    // free progs
    free(progs);

    return 0;
}
