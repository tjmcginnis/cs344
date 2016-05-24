#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/wait.h>

#define COMMAND_LENGTH 2048
#define MAX_ARGUMENTS 512
#define MAX_FORKS 100

size_t NUM_PROCESSES = 0;
pid_t* BACKGROUND_PROCESSES[MAX_FORKS];
size_t STATUS = 0;
/* 0 for exit status, positive integer for terminating signal */
size_t SIGNAL = 0;

struct Command {
    char* command;
    char* args[MAX_ARGUMENTS + 1];
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

    i = 1;

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
        // printf("token = %s | size = %lu\n", *token, strlen(*token));
        command->args[i] = malloc(sizeof(char) * strlen(*token)+1);
        memset(command->args[i], 0, strlen(*token)+1);
        memcpy(command->args[i], *token, strlen(*token));
        i = i + 1;
        *token = strtok(NULL, " ");
        if (*token == NULL) break;
        stat = regexec(&regex, *token, 0, NULL, 0);
    }

    regfree(&regex);
}

void set_io_redirect(struct Command* command, char** token)
{
    if (*token && strncmp(*token, "<", sizeof(*token)) == 0) {
        command->redirect = -1;
        *token = strtok(NULL, " ");
        command->input_file = malloc(sizeof(char) * sizeof(*token));
        memcpy(command->input_file, *token, sizeof(*token));
        *token = strtok(NULL, " ");
    } else if (*token && strncmp(*token, ">", sizeof(*token)) == 0) {
        command->redirect = 1;
        *token = strtok(NULL, " ");
        command->output_file = malloc(sizeof(char) * sizeof(*token));
        memcpy(command->output_file, *token, sizeof(*token));
        *token = strtok(NULL, " ");
    }
}

void set_background_flag(struct Command* command, char** token)
{
    if (*token && strncmp(*token, "&", sizeof(*token)) == 0) {
        command->background = 1;
    }
}

struct Command* command_create(char* input)
{
    size_t i;
    char* token;
    struct Command* command = malloc(sizeof(struct Command));
    assert(command != NULL);

    token = strtok(input, " ");

    /* get action */
    command->command = malloc(sizeof(char) * sizeof(token));
    memcpy(command->command, token, sizeof(token));

    command->args[0] = command->command;

    token = strtok(NULL, " ");

    command->input_file = 0;
    command->output_file = 0;
    command->redirect = 0;
    command->background = 0;

    /* parse args */
    for (i = 1; i < MAX_ARGUMENTS; i++) {
        command->args[i] = 0;
    }

    parse_args(command, &token);
    set_io_redirect(command, &token);
    set_background_flag(command, &token);

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

    // free(command->command);
    free(command);
}

void change_directory(char* path)
{
    char* tmp;
    char* dir;

    tmp = 0;
    dir = getenv("HOME");

    if (path != NULL) {
        if (path[0] == '/') {
            tmp = malloc(sizeof(char) * strlen(path));
            memset(tmp, 0, strlen(path));
            memcpy(tmp, path + 1, strlen(path) - 1);
            path = tmp;
        }
        dir = path;
    }

    chdir(dir);
    if (tmp) free(tmp);
}

void set_status(size_t new_status, size_t signal)
{
    STATUS = new_status;
    SIGNAL = signal;
}

void print_status()
{
    char* descriptor;
    if (SIGNAL == 0) {
        descriptor = "exit value";
    } else {
        descriptor = "terminated by signal";
    }
    printf("%s %zd\n", descriptor, STATUS);
}

void command_execute(struct Command* cmd)
{
    if (strncmp(cmd->command, "cd", strlen("cd")) == 0) {
        change_directory(cmd->args[1]);
    } else if (strncmp(cmd->command, "status", strlen("status")) == 0) {
        print_status();
    } else {
        //pid_t parent = getpid();
        pid_t pid = fork();

        if (pid == -1) {
            // error
        } else if (pid > 0) {
            //int status;
            //waitpid(pid, &status, 0);
            //printf("%d\n", status);
        } else {
            // child process
            // handle i/o redirect
            if (cmd->redirect == -1) {
                // input
                FILE* fd = fopen(cmd->input_file, "r");
                dup2(fileno(fd), STDIN_FILENO);
                fclose(fd);
            } else if (cmd->redirect == 1) {
                // output
                FILE* fd1 = fopen(cmd->output_file, "a");
                dup2(fileno(fd1), STDOUT_FILENO);
                fclose(fd1);
            }

            // handle background processes

            execvp(cmd->command, cmd->args);

            // exit status ?
            _exit(EXIT_FAILURE);
        }
        if (cmd->background == 0) {
            int stat;
            waitpid(pid, &stat, 0);
        } else {
            BACKGROUND_PROCESSES[NUM_PROCESSES] = pid;
            NUM_PROCESSES = NUM_PROCESSES + 1;
        }
    }
}

int main(int argc, char *argv[])
{
    size_t exit_flag;
    char command[COMMAND_LENGTH];
    struct Command* current_command;

    exit_flag = 0;

    do {
        get_input(command, COMMAND_LENGTH);
        if (command[0] == '\0' || command[0] == '#') continue;

        current_command = command_create(command);

        if (strncmp(current_command->command, "exit", strlen("exit")) == 0) {
            exit_flag = 1;
            command_destroy(current_command);
            break;
        }

        command_execute(current_command);
        command_destroy(current_command);

    } while (exit_flag == 0);

    /* kill any other processes or jobs */
    int counter;
    for (counter = 0; counter < NUM_PROCESSES; counter++) {
        printf("%zd\t", BACKGROUND_PROCESSES[NUM_PROCESSES]);
    }

    exit(0);
}
