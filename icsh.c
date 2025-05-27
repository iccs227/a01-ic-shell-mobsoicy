/* ICCS227: Project 1: icsh
 * Name: Keonhee Kim
 * StudentID: 6480994
 */

#include "stdio.h"
#include "headers/icsh.h"
#include "headers/signals.h"
#include "headers/execute.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"
#include <sys/wait.h>

pid_t fgpid = 0;
int last_status = 0;

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
        char expanded[MAX_CMD_BUFFER * 2] = "";

        // for !! command
        if (strstr(buffer, "!!")) {
            if (strlen(last_command)==0) {
                continue;
            }
            printf("%s\n", last_command);
            if (strchr(buffer, '>')) {
                char *redir = strchr(buffer, '>');
                strcpy(expanded, last_command);
                strcat(expanded, " ");
                strcat(expanded, redir);
                strcpy(last_command, expanded);
                execute(expanded);
                continue; // Skip to next iteration if redirection is present
            }
            strcpy(buffer, last_command); // Copy last command to buffer
        }
        else {
            strcpy(last_command, buffer); // Store the last command
        }

        if (strchr(buffer, '>')) {
            execute(buffer);
            continue; // Skip to next iteration if redirection is present
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
            execute(buffer);
        }
    }
}
