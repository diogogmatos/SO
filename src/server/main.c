#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "../../include/prog_info.h" // struct PROG

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        perror("Invalid number of arguments");
        return -1;
    }

    // folder where terminated programs state will be stored
    char *folder_path = argv[1];
    
    // create fifo
    int r = mkfifo("fifo", 0666);
    if (r == -1)
    {
        perror("mkfifo");
        return -1;
    }

    // open fifo
    int fd = open("fifo", O_RDONLY);
    if (fd == -1)
    {
        perror("open fifo");
        return -1;
    }

    // allows server to keep reading from fifo
    int keep_open = open("fifo", O_WRONLY);

    // read from fifo
    PROG p;
    int bytes_read;
    while ((bytes_read = read(fd, &p, sizeof(PROG))) > 0)
    {
        printf("bytes read: %d\n", bytes_read);
        printf("pid: %d\n", p.pid);
        printf("name: %s\n", p.name);
        printf("seconds: %ld\n", p.sec);
        printf("useconds: %ld\n", p.usec);

        if (p.pid == -1) break;

        // write execution time to client if pid is in logs
    }

    // close and unlink fifo
    close(fd);
    close(keep_open);
    unlink("fifo");
    unlink("keep_open");

    return 0;
}
