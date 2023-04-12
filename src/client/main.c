#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *option1 = argv[1];
    if (!strcmp(option1, "execute")) {
        char *option2 = argv[2];
        if (!strcmp(option2, "-u")) {
            // execução individual de um programa
            char *args = argv[3]; // argumentos do programa (inlui nome do programa em argv[0])
        }
        else if (!strcmp(option2, "-p")) {
            // execução de uma pipeline de programas
        }
        else {
            perror("Invalid option");
            return -1;
        }
    }
    else if (!strcmp(option1, "status")) {
        // modo status
    }
    else if (!strcmp(option1, "stats-time")) {
        // modo stats-time
    }
    else if (!strcmp(option1, "stats-command")) {
        // modo stats-command
    }
    else if (!strcmp(option1, "stats-uniq")) {
        // modo stats-uniq
    }
    else {
        perror("Invalid option");
        return -1;
    }

    // Open FIFO for writing
    int fd = open("fifo", O_WRONLY);

    if (fd == -1) {
        perror("open");
        return -1;
    }
}
