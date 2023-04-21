#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <sys/time.h>

#include "../../include/client/io.h"
#include "../../include/client/execute.h"

int handle_input(char *argv[])
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
        // modo status
    }
    else if (!strcmp(option1, "stats-time"))
    {
        // modo stats-time
    }
    else if (!strcmp(option1, "stats-command"))
    {
        // modo stats-command
    }
    else if (!strcmp(option1, "stats-uniq"))
    {
        // modo stats-uniq
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
}
