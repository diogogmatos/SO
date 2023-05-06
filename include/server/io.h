#ifndef io_h
#define io_h

int send_status(GHashTable *log);
int handle_message(GHashTable *log, MESSAGE m, char *folder_path);

#endif
