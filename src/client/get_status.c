#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <glib.h>

#include "../../include/client/get_status.h"
#include "../../include/message.h"
#include "../../include/status.h"

int get_status() {
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
