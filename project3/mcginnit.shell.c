#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "builtins.h"

#define COMMAND_LENGTH 2048
#define MAX_ARGUMENTS 512


int main(int argc, char *argv[])
{
    char* token;
    size_t exit_length = strlen("exit");
    char command[COMMAND_LENGTH];

    char* action;

    do {
        printf(": ");
        fgets(command, COMMAND_LENGTH, stdin);

        token = strtok(command, "\n");
        token = strtok(token, " ");
        action = token;
        token = strtok(NULL, " ");

        if (strncmp(action, "cd", strlen("cd")) == 0) {
            change_directory(token);
        } else if (strncmp(action, "status", strlen("status")) == 0) {
            printf("%d\n", get_status());
        } else if (strncmp(action, "exit", strlen("exit")) == 0) {
            printf("exit command issued\n");
        } else if (strncmp(action, "#", strlen("#")) == 0) {
        } else {
            printf("Non built-in command issued\n");
        }

        fflush(stdin);
    } while (strncmp(command, "exit", exit_length) != 0);

    /* kill any other processes or jobs */

    exit(0);
}
