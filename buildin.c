#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "headers/icsh.h"
#include "headers/buildin.h"
#include "headers/execute.h"
#include "headers/animated.h"

int buildin_handler(char *buffer, int *last_status, char *last_command, char *expanded) {
    /// for !! command
    if (strstr(buffer, "!!")) {
        if (strlen(last_command)==0) {
            *last_status = 0;
            return 1; // 1 for buildin commands
        }
        printf("%s\n", last_command);

        if (strchr(buffer, '>')) {
            char *redir = strchr(buffer, '>');
            strcpy(expanded, last_command);
            strcat(expanded, " ");
            strcat(expanded, redir);
            *last_status = 0;
            return 2; // 2 for commands with redirection
        }
        strcpy(buffer, last_command); // Copy last command to buffer
        *last_status = 0;
    }

    // for jobs command
    if (strcmp(buffer, "jobs") == 0) {
        if (job_count == 0) {
            printf("No jobs running\n");
            return 1; // 1 for buildin commands
        } 
        else {
            job_handler(LIST_JOBS, 0, NULL, NULL); // List all jobs
            return 1; // 1 for buildin commands
        }
        *last_status = 0;
    }

    if (strncmp(buffer, "fg %", 4) == 0) {
        // For after % sign
        int job_id = atoi(buffer+4);
        for (int i=0; i<job_count; i++) {
            if (jobs[i].job_id == job_id) {
                fgpid = jobs[i].pid;

                if (strcmp(jobs[i].status, "Stopped") == 0) {
                    kill(jobs[i].pid, SIGCONT);
                }
                // update to running
                job_handler(UPDATE_JOB, jobs[i].pid, NULL, "Running");
                printf("%s\n", jobs[i].command);

                // wait for job to finish
                int status;
                waitpid(jobs[i].pid, &status, WUNTRACED);
                fgpid = 0;
                if (WIFEXITED(status)) {
                    job_handler(REMOVE_JOB, jobs[i].pid, NULL, NULL);
                }
                else if (WIFSIGNALED(status)) {
                    job_handler(REMOVE_JOB, jobs[i].pid, NULL, NULL);
                }
                else if (WIFSTOPPED(status)) {
                    job_handler(UPDATE_JOB, jobs[i].pid, NULL, "Stopped");
                }
                *last_status = 0;
                return 1;
            }
        }
        printf("job not found\n");
        *last_status = 0;
        return 1;
    }

    if (strncmp(buffer, "bg %", 4)== 0) {
        // For after % sign
        int job_id = atoi(buffer+4);
        for (int i=0; i<job_count; i++) {
            if (jobs[i].job_id == job_id) {
                if (strcmp(jobs[i].status, "Stopped") == 0) {
                    kill(jobs[i].pid, SIGCONT);
                    job_handler(UPDATE_JOB, jobs[i].pid, NULL, "Running");
                    printf("[%d] %s &\n", jobs[i].job_id, jobs[i].command);
                }
                *last_status = 0;
                return 1;
            }
        }
        printf("job not found\n");
        *last_status = 0;
        return 1;
    }

    // for echo $? command
    if (strcmp(buffer, "echo $?") == 0) {
        printf("%d\n", *last_status);
        *last_status = 0;
        return 1;
    }

    // for echo command
    else if (strncmp(buffer, "echo ", 5)==0) {
        char *redir = strchr(buffer, '>');
        if (redir) {
            strcpy(expanded, buffer);
            *last_status = 0;
            return 2;
        }
        printf("%s\n", buffer+5);
        *last_status = 0;
        return 1;
    }

    // for exit command
    else if (strncmp(buffer, "exit ", 5)==0) {
        int exit_code = atoi(buffer+5) & 0xFF;
        print_animated_end();
        exit(exit_code);
    }

    if (strchr(buffer, '&')) {
        char *symbol = strchr(buffer, '&');
        *symbol = '\0';
        execute(buffer, 1);
        *last_status = 0;
        return 1;
    }

    return 0;
}

void job_handler(Job_Action action, pid_t pid, char *command, char *status) {
    switch (action) {
        case ADD_JOB:
            // add job to list
            if (job_count < MAX_JOBS) {
                jobs[job_count].job_id = next_job_id++;
                jobs[job_count].pid = pid;
                strncpy(jobs[job_count].command, command, MAX_CMD_BUFFER-1);
                jobs[job_count].command[MAX_CMD_BUFFER -1] = '\0';
                strcpy(jobs[job_count].status, status);
                job_count++;
            }
            else {
                printf("job list is full\n");
            }
            break;

        case UPDATE_JOB:
            // update status of job
            for (int i=0; i<job_count; i++) {
                if (jobs[i].pid == pid) {
                    strcpy(jobs[i].status, status);
                    return;
                }
            }
            break;

        case REMOVE_JOB:
            // remove job from list
            for (int i=0; i<job_count; i++) {
                if (jobs[i].pid == pid) {
                    for (int j=i; j<job_count-1; j++) {
                        jobs[j] = jobs[j+1]; // shift jobs to left (down)
                    }
                    job_count--;

                    if (job_count == 0) {
                        next_job_id = 1; // Reset job ID if no jobs left
                    }
                    return;
                }
            }
            next_job_id = 1;
            break;

        case LIST_JOBS:
            // list all jobs
            for (int i=0; i<job_count; i++) {
                printf("[%d]  %s                 %s\n", jobs[i].job_id, jobs[i].status, jobs[i].command);
            }
            break;

        default:
            return; // No action
    }
}