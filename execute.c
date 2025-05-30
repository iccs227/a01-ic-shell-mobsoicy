#include "headers/icsh.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"
#include <sys/wait.h>
#include "fcntl.h"
#include "headers/buildin.h"

void execute(char *buffer, int is_background) {
    if (strcmp(buffer, "")==0) {
        return; // Ignore empty command
    }

    char original_buffer[MAX_CMD_BUFFER];
    strncpy(original_buffer, buffer, MAX_CMD_BUFFER-1);
    original_buffer[MAX_CMD_BUFFER-1] = '\0';

    // parse redirection
    char *in_file = NULL;
    char *out_file = NULL;
    char *in_ptr = strchr(buffer, '<');
    char *out_ptr = strchr(buffer, '>');

    if (in_ptr) {
        *in_ptr = '\0'; // Split the command
        in_ptr++;
        in_file = strtok(in_ptr, " \t");
    }
    if (out_ptr) {
        *out_ptr = '\0'; // Split the command
        out_ptr++;
        out_file = strtok(out_ptr, " \t");
    }

    char *args[MAX_CMD_BUFFER];
    int i = 0;
    args[i] = strtok(buffer, " ");
    while (args[i] != NULL) {
        i++;
        args[i] = strtok(NULL, " ");
    }

    // fork a child process
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        return;
    }
    
    if (pid == 0) { // child process
        if (in_file) {
            int fd = open(in_file, O_RDONLY);
            if (fd < 0) {
                perror("Input file does not exist");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        if (out_file) {
            int fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);
        setpgid(0, 0);

        execvp(args[0], args);
        perror("Command execution failed");
        exit(1);
    } 
    else { // parent process
        if (is_background) {
            job_handler(ADD_JOB, pid, original_buffer, "Running");
            printf("[%d] %d\n", next_job_id-1, pid);
        }
        else {
            fgpid = pid;
            int status;
            waitpid(pid, &status, WUNTRACED);
            fgpid = 0;
            if (WIFEXITED(status)) {
                last_status = WEXITSTATUS(status);
            }
            else if (WIFSIGNALED(status)) {
                last_status = WTERMSIG(status) + 128;
            }
            else if (WIFSTOPPED(status)) {
                last_status = WSTOPSIG(status) + 128;
            }
        }
    }
}