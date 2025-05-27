#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int buildin_handler(char *buffer, int *last_status, char *last_command, char *expanded) {
    /// for !! command
    if (strstr(buffer, "!!")) {
        if (strlen(last_command)==0) {
            return 1; // 1 for buildin commands
        }
        printf("%s\n", last_command);

        if (strchr(buffer, '>')) {
            char *redir = strchr(buffer, '>');
            strcpy(expanded, last_command);
            strcat(expanded, " ");
            strcat(expanded, redir);
            return 2; // 2 for commands with redirection
        }
        strcpy(buffer, last_command); // Copy last command to buffer
    }

    // for echo $? command
    if (strcmp(buffer, "echo $?") == 0) {
        printf("%d\n", *last_status);
        return 1;
    }

    // for echo command
    else if (strncmp(buffer, "echo ", 5)==0) {
        char *redir = strchr(buffer, '>');
        if (redir) {
            strcpy(expanded, buffer);
            return 2;
        }
        printf("%s\n", buffer+5);
        return 1;
    }

    // for exit command
    else if (strncmp(buffer, "exit ", 5)==0) {
        int exit_code = atoi(buffer+5) & 0xFF;
        printf("bye\n");
        exit(exit_code);
    }

    return 0;
}