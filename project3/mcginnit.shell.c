#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define COMMAND_LENGTH 2048
#define MAX_ARGUMENTS 512

static int STATUS;


void change_directory()
{

}

int main(int argc, char *argv[])
{
    char* token;
    size_t exit_length = strlen("exit");
    char command[COMMAND_LENGTH];

    char* action;
    char* arg;

    do {
        printf(": ");
        fgets(command, COMMAND_LENGTH, stdin);

        token = strtok(command, "\n");
        token = strtok(token, " ");
        action = token;
        token = strtok(NULL, " ");

        if (strncmp(action, "cd", strlen("cd")) == 0) {
            printf("cd command issued\n");
            char* path = getenv("HOME");
            if (token != NULL) {
                path = token;
            }
            chdir(path);

            char cwd[1024];
            getcwd(cwd, sizeof(cwd));
            printf("Current dir: %s\n", cwd);

        } else if (strncmp(action, "status", strlen("status")) == 0) {
            printf("status command issued\n");
        } else if (strncmp(action, "exit", strlen("exit")) == 0) {
            printf("exit command issued\n");
        } else {
            printf("Non built-in command issued\n");
        }

        fflush(stdin);
    } while (strncmp(command, "exit", exit_length) != 0);

    /* kill any other processes or jobs */

    exit(0);
}
