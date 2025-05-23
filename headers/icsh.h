#include <sys/wait.h>
#include "stdio.h"

#define MAX_CMD_BUFFER 255
extern pid_t fgpid;
extern int last_status;
void trim_line(char *buffer);