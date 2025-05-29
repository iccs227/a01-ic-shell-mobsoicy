#include <sys/wait.h>
#include "stdio.h"

#define MAX_CMD_BUFFER 255
#define MAX_JOBS 100
extern pid_t fgpid;
extern int last_status;
extern int next_job_id;

typedef struct {
    int job_id;
    pid_t pid;
    char status[16];
    char command[MAX_CMD_BUFFER];
} Job;

extern Job jobs[MAX_JOBS];
extern int job_count;
extern char last_command[MAX_CMD_BUFFER];