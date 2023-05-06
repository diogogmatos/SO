#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <glib.h>

#include "../../include/message.h" // struct MESSAGE
#include "../../include/status.h" // struct STATUS
#include "../../include/global_utils.h"
#include "../../include/server/io.h"

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

    // create hash table to store program data, with pid as hash key
    GHashTable *log = g_hash_table_new(g_int_hash, g_int_equal);

    // read from fifo
    MESSAGE m;
    int bytes_read;
    while ((bytes_read = read(fd, &m, sizeof(MESSAGE))) > 0 && m.type != e_quit_server)
    {
        printf("pid: %d\n", m.pid);
        printf("type: %d\n", m.type);
        printf("timestamp: %ld\n", m.timestamp);
        printf("message: %s\n", m.message);
        printf("\n");

        handle_message(log, m, folder_path);
    }

    // close and unlink fifo
    close(fd);
    close(keep_open);
    unlink("fifo");
    unlink("keep_open");

    // free hash table
    g_hash_table_destroy(log);

    return 0;
}
