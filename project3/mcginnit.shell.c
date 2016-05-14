#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "builtins.h"

#define COMMAND_LENGTH 2048
#define MAX_ARGUMENTS 512
#define MAX_FORKS 100


struct Shell {
    pid_t* background_processes[MAX_FORKS];
    char* cwd;  // current working directory
    size_t status;
    size_t last_exit;  // 0 for 'exit status', 1 for 'terminating signal'
};

struct Command {
    char* command;
    char* args[MAX_ARGUMENTS];
    size_t redirect;  // 0 for none, -1 for input, 1 for output
    char* input_file;
    char* output_file;
    size_t background;  // 0 for foreground process, 1 for background process
};

void get_input(char* buffer, size_t length)
{
    printf(": ");
    fgets(buffer, length, stdin);

    // replace trailing newline null terminator
    buffer[strlen(buffer) - 1] = 0;

    // flush stdin for the next command
    fflush(stdin);
}

struct Command* command_create(char* input){
    char* token;
    struct Command* command = malloc(sizeof(struct Command));
    assert(command != NULL);

    token = strtok(input, " ");

    /* get action */
    command->command = malloc(sizeof(char) * strlen(token));
    memcpy(command->command, token, strlen(token));

    token = strtok(NULL, " ");
    /* parse args */

    /* i/o redirection */

    return command;
}

void command_destroy(struct Command* command)
{
    assert(command != NULL);
    assert(command->command != NULL);

    free(command->command);
    free(command);
}

void parse_args(struct Command* cmd, char* token)
{
    
}
/*
void command_execute(struct Command* cmd){}
*/

int main(int argc, char *argv[])
{
    // char* token;
    char command[COMMAND_LENGTH];
    struct Command* current_command;

    // char* action;

    do {
        get_input(command, COMMAND_LENGTH);
        current_command = command_create(command);
        /*
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
        }*/
        command_destroy(current_command);

    } while (strncmp(command, "exit", strlen("exit")) != 0);

    /* kill any other processes or jobs */

    exit(0);
}
