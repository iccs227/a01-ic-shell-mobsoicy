#include "headers/icsh.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"
#include <sys/wait.h>

void execute(char *buffer) {
    if (strcmp(buffer, "")==0) {
        return; // Ignore empty command
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
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        execvp(args[0], args);
        perror("Command execution failed");
        exit(1);
    } 
    else { // parent process
        int status;
        fgpid = pid;
        waitpid(pid, &status, WUNTRACED);
        fgpid = 0;
        if (WIFEXITED(status)) {
            last_status = WEXITSTATUS(status);
        } 
        else if (WIFSIGNALED(status)) {
            last_status = WTERMSIG(status);
            printf("\n");
        } 
        else if (WIFSTOPPED(status)) {
            printf("\n");
        }
    }
}