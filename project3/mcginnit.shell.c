#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define COMMAND_LENGTH 2048

int main(int argc, char *argv[])
{
    size_t exit_length = strlen("exit");
    char command[COMMAND_LENGTH];

    do {
        printf(": ");
        fgets(command, COMMAND_LENGTH, stdin);
        fflush(stdin);
    } while (strncmp(command, "exit", exit_length) != 0);

    exit(0);
}
