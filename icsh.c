/* ICCS227: Project 1: icsh
 * Name: Keonhee Kim
 * StudentID: 6480994
 */

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define MAX_CMD_BUFFER 255

int main() {
    char buffer[MAX_CMD_BUFFER];
    char last_command[MAX_CMD_BUFFER] = ""; // To store the last command
    printf("Starting IC shell\n");
    while (1) {
        printf("icsh $ ");
        fgets(buffer, MAX_CMD_BUFFER, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline character
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

        // for echo command
        if (strncmp(buffer, "echo ", 5)==0) {
            printf("%s\n", buffer+5);
        }

        // for exit command
        if (strncmp(buffer, "exit ", 5)==0) {
            int exit_code = atoi(buffer+5) & 0xFF;
            printf("bye\n");
            exit(exit_code);
        }

        // for bad comand
        else {
            if (strcmp(buffer, "")==0) {
                continue; // Ignore empty command
            }
            printf("bad command\n");
        }
    }
}
