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
#include "headers/buildin.h"

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

        // handle buildin commands
        int build_in = buildin_handler(buffer, &last_status, last_command, expanded);
        if (build_in == 1) {
            strcpy(last_command, buffer);
            continue;
        }
        else if (build_in == 2) {
            strcpy(last_command, expanded);
            execute(expanded);
            continue;
        }
        strcpy(last_command, buffer);
        execute(buffer); // for external commands
    }
}
