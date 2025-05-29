#include "unistd.h"
#include "headers/icsh.h"
#include "headers/buildin.h"
#include <sys/wait.h>
#include "string.h"

void sigint_handler(int signo) {
    if (fgpid > 0) {
        kill(fgpid, SIGINT);
        printf("\n");
    }
}

extern char last_command[MAX_CMD_BUFFER];
void sigtstp_handler(int signo) {
    if (fgpid > 0) {
        kill(fgpid, SIGTSTP);
        int job_id = 0;
        for (int i=0; i<job_count; i++) {
            if (jobs[i].pid == fgpid) {
                job_id = jobs[i].job_id;
                job_handler(UPDATE_JOB, fgpid, NULL, "Stopped");
                break;
            }
        }

        if (job_id > 0) {
            printf("\n[%d]  Stopped                 %s\n", job_id, jobs[job_id-1].command);
        }
        else {
            job_handler(ADD_JOB, fgpid, last_command, "Stopped");
            printf("\n[%d]  Stopped                 %s\n", next_job_id-1, last_command);
        }
    }
}

void sigchld_handler(int signo) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            for (int i =0; i<job_count; i++) {
                if (jobs[i].pid == pid) {
                    job_handler(UPDATE_JOB, jobs[i].pid, NULL, "Done");
                    break;
                }
            }
        }
    }
}