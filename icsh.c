/* ICCS227: Project 1: icsh
 * Name: Keonhee Kim
 * StudentID: 6480994
 */

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "signal.h"
#include "unistd.h"
#include <sys/wait.h>

#define MAX_CMD_BUFFER 255

pid_t fgpid = 0;
int last_status = 0;

void sigint_handler(int signo) {
    if (fgpid > 0) {
        kill(fgpid, SIGINT);
    }
}

void sigtstp_handler(int signo) {
    if (fgpid > 0) {
        kill(fgpid, SIGTSTP);
    }
}

int main(int argc, char *argv[]) {
    char buffer[MAX_CMD_BUFFER];
    char last_command[MAX_CMD_BUFFER] = ""; // To store the last command
    FILE *script_file = NULL;

    if (argc == 2) {
        script_file = fopen(argv[1], "r");
        if (script_file == NULL) {
            perror("Error opening script file %s\n");
            return 1;
        }
    }

    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);

    printf("Starting IC shell\n");
    while (1) {
        if (script_file) {
            if (fgets(buffer, MAX_CMD_BUFFER, script_file) == NULL) {
                fclose(script_file);
                script_file = NULL;
                continue;
            }
        }
        else {
            printf("icsh $ ");
            fgets(buffer, MAX_CMD_BUFFER, stdin);
        }

        buffer[strcspn(buffer, "\r\n")] = 0; // Remove newline character

        // for !! command
        if (strcmp(buffer, "!!")==0) {
            if (strlen(last_command)==0) {
                continue;
            }
            printf("%s\n", last_command);
            strcpy(buffer, last_command); // Copy last command to buffer
        }
        else {
            strcpy(last_command, buffer); // Store the last command
        }

        // for echo $? command
        if (strcmp(buffer, "echo $?") == 0) {
            printf("%d\n", last_status);
        }

        // for echo command
        else if (strncmp(buffer, "echo ", 5)==0) {
            printf("%s\n", buffer+5);
        }

        // for exit command
        else if (strncmp(buffer, "exit ", 5)==0) {
            int exit_code = atoi(buffer+5) & 0xFF;
            printf("bye\n");
            exit(exit_code);
        }

        // external code
        else {
            if (strcmp(buffer, "")==0) {
                continue; // Ignore empty command
            }
            // printf("bad command\n");

            // parse command and arguments
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
                continue;
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
    }
}
