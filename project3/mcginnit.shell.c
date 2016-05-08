#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define COMMAND_LENGTH 2048
#define MAX_ARGUMENTS 512

static int STATUS;

int main(int argc, char *argv[])
{
    char* token;
    size_t exit_length = strlen("exit");
    char command[COMMAND_LENGTH];

    do {
        printf(": ");
        fgets(command, COMMAND_LENGTH, stdin);
        token = strtok(command, " ");
        while (token != NULL ) {
            printf( "%s\n", token );
            token = strtok(NULL, " ");
        }
        fflush(stdin);
    } while (strncmp(command, "exit", exit_length) != 0);

    char cwd[1024];
    chdir("..");
    getcwd(cwd, sizeof(cwd));
    printf("Current dir: %s\n", cwd);

    const char *home;
    home = getenv("HOME");
    chdir(home);
    getcwd(cwd, sizeof(cwd));
    printf("Current dir: %s\n", cwd);


    /* kill any other processes or jobs */

    exit(0);
}
