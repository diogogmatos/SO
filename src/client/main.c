#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>

#include "../../include/client/io.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        perror("Invalid number of arguments");
        return -1;
    }

    // handle input
    int input_res = handle_input(argc, argv);
    
    if (input_res == -1)
    {
        perror("Invalid option");
        return -1;
    }

    return 0;
}
