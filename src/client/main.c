#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>

#include "../../include/client/io.h"

int main(int argc, char *argv[])
{
    // handle input
    int input_res = handle_input(argv);
    
    if (input_res == -1)
    {
        perror("Invalid option");
        return -1;
    }

    return 0;
}
