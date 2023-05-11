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

int function (char **argv, int pid_in) 
{
    int i;
    for (i = 0; argv[i] != NULL && strcmp("|", argv[i]); i++);

    if (argv[i] != NULL)
    {
        argv[i] = NULL;

        int fildes[2];

        int status_fildes = pipe(fildes);

        if (status_fildes == -1) 
        {
            perror("pipe");
            return -1;
        }

        int pid = fork();

        // child
        if (pid == 0)
        {
            // não iremos usar o pipe de writing neste child
            close(fildes[1]);

            // passamos como argumeno o pipe de reading criado
            function(&argv[i+1], fildes[0]);

            // estamos no child. o child não pode esperar pelo pai.
            // por isso, o wait não funciona aqui
            // o flow irá prosseguir quando o function acabar

            // int status;
            // int r = wait(&status);
            // if (r == -1)
            // {
            //     perror("wait");
            //     return -1;
            // }
            
            // não irá mais ser usado o pipe de reading
            close(fildes[1]);

        }
        // parent
        else
        {
            // trocar o stdin pelo pipe de reading dado
            int in_dup_status = dup2(0, pid_in);

            if (in_dup_status == -1) 
            {
                perror("dup");
                return -1;
            }

            // trocar o stdout pelo pipe de writing criado
            int out_dup_status = dup2(1, fildes[1]);

            if (out_dup_status == -1) 
            {
                perror("dup");
                return -1;
            }

            int exec_res = execvp(argv[0], &argv[0]);

            if (exec_res == -1)
            {
                perror("execvp");
            }

            // quando o programa termina os field descriptors
            // são fechados automaticamente

            // close(fildes[0]);
            // close(fildes[1]);

            exit(0);

        }

    }
    else 
    {

        int pid = fork();

        // parent
        if (pid != 0)
        {
            // como agora estamos na última chamada recursiva
            // todas as outras terão de esperar que este wait responda
            // (porque este pedaço de código faz parte do main flow do programa)
            // que pode existir porque agora estamos no parent
            // que pode acontecer porque agora não nos temos de preocupar 
            // com parent-child read-writes que acontece acima 

            int status;
            int r = wait(&status);
            if (r == -1)
            {
                perror("wait");
                return -1;
            }
        }
        // parent
        else
        {

            // trocar o stdin pelo pipe de reading dado
            int in_dup_status = dup2(0, pid_in);

            if (in_dup_status == -1) 
            {
                perror("dup");
                return -1;
            }

            // o stdout não é alterado

            int exec_res = execvp(argv[0], &argv[0]);

            if (exec_res == -1)
            {
                perror("execvp");
            }

            exit(0);
        }
        
    }

    return 0;
}

int execute_p(char *args)
{
    int size;
    char **argv = str_to_array(args, " ", &size);

    int pid = fork();
    
    // child
    if (pid == 0)
    {
        function(argv, 0);

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
