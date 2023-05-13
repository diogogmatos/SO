#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>

#include "../../include/client/io.h"

#include "../../include/client/execute.h"
#include "../../include/message.h"
#include "../../include/client/stats.h"
#include "../../include/status.h"

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

int status() {
    // make response fifo
    int r = mkfifo("response_fifo", 0666);
    if (r == -1)
    {
        perror("mkfifo response_fifo");
        return -1;
    }

    // open fifo
    int fd = open("fifo", O_WRONLY);
    if (fd == -1)
    {
        perror("open fifo");
        return -1;
    }

    // add message info to struct
    MESSAGE m = {0};
    m.type = e_status;
    strncpy(m.message, "STATUS", MESSAGE_SIZE);

    // write to fifo
    int w = write(fd, &m, sizeof(MESSAGE));
    if (w == -1)
    {
        perror("write status");
        return -1;
    }

    close(fd);

    // open response fifo
    int fd_response = open("response_fifo", O_RDONLY);
    if (fd_response == -1)
    {
        perror("open response fifo");
        return -1;
    }

    // read from response fifo
    STATUS s;
    int bytes_read;
    while ((bytes_read = read(fd_response, &s, sizeof(STATUS))) > 0 && s.pid > 0) {
        printf("%d %s %g ms\n", s.pid, s.name, s.execution_time);
    }

    // close and unlink response fifo
    close(fd_response);
    unlink("response_fifo");

    return 0;
}

int handle_input(int argc, char *argv[])
{
    char *option1 = argv[1];

    // execução de programas
    if (!strcmp(option1, "execute"))
    {
        char *option2 = argv[2];

        // execução individual de um programa
        if (!strcmp(option2, "-u"))
        {
            char *args = argv[3]; // argumentos do programa (inlui nome do programa em argv[0])

            int r = execute_u(args);
            if (r == -1)
            {
                perror("execute_u()");
                return -1;
            }
        }

        // execução de uma pipeline de programas
        else if (!strcmp(option2, "-p"))
        {
            char *args = argv[3]; // argumentos de cada programa, separados por '|'

            int r = execute_p(args);
            if (r == -1)
            {
                perror("execute_p()");
                return -1;
            }
        }

        // opção inválida
        else
        {
            perror("invalid option2");
            return -1;
        }
    }
    else if (!strcmp(option1, "status"))
    {
        int r = status();
        if (r == -1)
        {
            perror("status()");
            return -1;
        }
    }
    else if (!strcmp(option1, "stats-time"))
    {
        char **args = &argv[2];
        int nr = argc - 2;
        int r = stats_time(nr, args);
        if (r == -1)
        {
            perror("stats_time()");
            return -1;
        }
    }
    else if (!strcmp(option1, "stats-command"))
    {
        char **args = &argv[3];
        int nr = argc - 3;
        char *command = argv[2];
        int r = stats_command(nr, args, command);
        if (r == -1)
        {
            perror("stats_command()");
            return -1;
        }
    }
    else if (!strcmp(option1, "stats-uniq"))
    {
        char **args = &argv[2];
        int nr = argc - 2;
        int r = stats_uniq(nr, args);
        if (r == -1)
        {
            perror("stats_uniq()");
            return -1;
        }
    }
    else if (!strcmp(option1, "stop"))
    {
        int r = quit_server();
        if (r == -1)
        {
            perror("quit_server()");
            return -1;
        }
    }
    // opção inválida
    else
    {
        perror("invalid option1");
        return -1;
    }

    return 0;
}
