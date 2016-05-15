#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <regex.h>

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

void parse_args(struct Command* command, char** token)
{
    size_t i, stat;
    regex_t regex;

    i = 0;

    stat = regcomp(&regex, "[<|>|&]", REG_NOSUB);
    assert(stat == 0);

    /* NULL token will cause seg fault */
    if (*token == NULL) {
        regfree(&regex);
        return;
    }

    stat = regexec(&regex, *token, 0, NULL, 0);
    while (stat && i < MAX_ARGUMENTS) {
        // add token to array of arguments
        command->args[i] = malloc(sizeof(char) * sizeof(*token));
        memcpy(command->args[i], *token, sizeof(*token));
        i = i + 1;
        *token = strtok(NULL, " ");
        if (*token == NULL) break;
        stat = regexec(&regex, *token, 0, NULL, 0);
    }

    regfree(&regex);
}

struct Command* command_create(char* input){
    size_t i;
    char* token;
    struct Command* command = malloc(sizeof(struct Command));
    assert(command != NULL);

    token = strtok(input, " ");

    /* get action */
    command->command = malloc(sizeof(char) * sizeof(token));
    memcpy(command->command, token, sizeof(token));

    command->input_file = 0;
    command->output_file = 0;

    token = strtok(NULL, " ");

    /* parse args */
    for (i = 0; i < MAX_ARGUMENTS; i++) {
        command->args[i] = 0;
    }

    parse_args(command, &token);

    /* i/o redirection */
    command->redirect = 0;
    if (token && strncmp(token, "<", sizeof(token)) == 0) {
        command->redirect = -1;
        token = strtok(NULL, " ");
        command->input_file = malloc(sizeof(char) * sizeof(token));
        memcpy(command->input_file, token, sizeof(token));
    } else if (token && strncmp(token, ">", sizeof(token)) == 0) {
        command->redirect = 1;
        token = strtok(NULL, " ");
        command->output_file = malloc(sizeof(char) * sizeof(token));
        memcpy(command->output_file, token, sizeof(token));
    }

    /* background process */
    command->background = 0;
    if (token && strncmp(token, "&", sizeof(token)) == 0) {
        printf("run process in background\n");
        command->background = 1;
    }

    return command;
}

void command_destroy(struct Command* command)
{
    size_t i;
    assert(command != NULL);
    assert(command->command != NULL);

    for (i = 0; command->args[i]; i++) {
        free(command->args[i]);
    }

    if (command->input_file) {
        free(command->input_file);
    }
    if (command->output_file) {
        free(command->output_file);
    }

    free(command->command);
    free(command);
}

/*
void command_execute(struct Command* cmd){}
*/

int main(int argc, char *argv[])
{
    // char* token;
    size_t exit_flag;
    char command[COMMAND_LENGTH];
    struct Command* current_command;

    // char* action;

    exit_flag = 0;

    do {
        get_input(command, COMMAND_LENGTH);
        if (command[0] == '\0' || command[0] == '#') continue;

        current_command = command_create(command);

        if (strncmp(current_command->command, "exit", strlen("exit")) == 0) {
            exit_flag = 1;
        }

        /*
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

    } while (exit_flag == 0);

    /* kill any other processes or jobs */

    exit(0);
}
