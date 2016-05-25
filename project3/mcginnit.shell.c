#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <regex.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define COMMAND_LENGTH 2048
#define MAX_ARGUMENTS 512
#define MAX_FORKS 100

int NUM_PROCESSES = 0;
pid_t BACKGROUND_PROCESSES[MAX_FORKS];
int STATUS = 0;
/* 0 for exit status, positive integer for terminating signal */
char STATUS_MESSAGE[64] = "exit value 0";

char* MESSAGE_QUEUE[MAX_FORKS];
int NUM_MESSAGES = 0;
int error_flag = 0;

struct Command {
    char* command;
    char* args[MAX_ARGUMENTS + 1];
    int redirect;  // 0 for none, -1 for input, 1 for output
    char* input_file;
    char* output_file;
    int background;  // 0 for foreground process, 1 for background process
};


void print_and_flush(char* buffer)
{
    write(STDOUT_FILENO, buffer, strlen(buffer));
    fflush(stdout);
}

void get_input(char* buffer, int length)
{
    print_and_flush(": ");
    fgets(buffer, length, stdin);

    // replace trailing newline null terminator
    buffer[strlen(buffer) - 1] = 0;

    // flush stdin for the next command
    fflush(stdin);
}

void parse_args(struct Command* command, char** token)
{
    int i, stat;
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
    int i;
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
    int i;
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

void print_status()
{
    char msg[64];
    sprintf(msg, "%s\n", STATUS_MESSAGE);
    print_and_flush(msg);
}

void set_status(char* new_status)
{
    memset(STATUS_MESSAGE, 0, 64);
    memcpy(STATUS_MESSAGE, new_status, strlen(new_status));
}

void add_to_message_queue(char* message)
{
    MESSAGE_QUEUE[NUM_MESSAGES] = malloc(strlen(message));
    memset(MESSAGE_QUEUE[NUM_MESSAGES], 0, sizeof(MESSAGE_QUEUE[NUM_MESSAGES]));
    memcpy(MESSAGE_QUEUE[NUM_MESSAGES], message, strlen(message));
    NUM_MESSAGES = NUM_MESSAGES + 1;
}

void background_exit(int signum)
{
    int stat;
    pid_t pid;

    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
        if (pid == -1)
            return;
        else {
            char message[128];
            sprintf(message, "background pid %ld is done: signal %d \n", (long)pid, stat);
            // add to message queue
            add_to_message_queue(message);
            // write(STDOUT_FILENO, message, strlen(message));
        }
    }
}

void redirect_input(struct Command* cmd)
{
    /* open file */
    FILE* fd = fopen(cmd->input_file, "r");

    if (fd == NULL) {
        /* file does not exist */
       char msg[64];
        sprintf(msg, "cannot open %s for input\n", cmd->input_file);
        print_and_flush(msg);
        set_status("exit value 1");  // set shell exit status
        /* set error_flag so command does not exec */
        error_flag = 1;
        return;
    }
    dup2(fileno(fd), STDIN_FILENO);
    fclose(fd);
}

void redirect_output(struct Command* cmd)
{
    FILE* fd = fopen(cmd->output_file, "w");
    if (fd == NULL) {
        /* file does not exist */
        char msg[64];
        sprintf(msg, "cannot open %s for output\n", cmd->output_file);
        print_and_flush(msg);
        set_status("exit value 1");  // set shell exit status
        /* set error_flag so command does not exec */
        error_flag = 1;
        return;
    }
    dup2(fileno(fd), STDOUT_FILENO);
    fclose(fd);
}

void wait_foreground_command(pid_t pid)
{
    int status;
    int exit_value;
    char status_message[64];

    /* wait for execution to complete and get exit value */
    waitpid(pid, &status, 0);
    exit_value = WEXITSTATUS(status);

    /* set status */
    sprintf(status_message, "exit value %d", exit_value);
    set_status(status_message);
}

void sigint_handler(int sig)
{
    print_and_flush("terminated by signal 2\n");
    set_status("terminated by signal 2");
    error_flag = 1;
}

void command_execute(struct Command* cmd)
{
    error_flag = 0;

    if (strncmp(cmd->command, "cd", strlen("cd")) == 0) {
        change_directory(cmd->args[1]);
    } else if (strncmp(cmd->command, "status", strlen("status")) == 0) {
        print_status();
    } else {
        pid_t pid;

        /* set up signal handling for foreground processes */
        if (cmd->background == 0) {
            struct sigaction sa;
            sa.sa_handler = sigint_handler;
            sa.sa_flags = 0;
            sigemptyset(&sa.sa_mask);

            if (sigaction(SIGINT, &sa, NULL) == -1) {
                perror("sigaction");
                exit(1);
            }
        }

        if (NUM_PROCESSES < 100) {
            pid = fork();
            NUM_PROCESSES = NUM_PROCESSES + 1;
        } else {
            exit(1);
        }

        if (pid == -1) {
            // error
        } else if (pid > 0) {
            //int status;
            //waitpid(pid, &status, 0);
            //printf("%d\n", status);
            // printf("%zd", cmd->background);
        } else {
            // child process

            if (cmd->redirect == -1) {
                redirect_input(cmd);
            } else if (cmd->redirect == 1) {
                redirect_output(cmd);
            }

            printf("ERROR FLAG: %d\n", error_flag);
            if (error_flag == 0) {
                if (execvp(cmd->command, cmd->args) == -1) {
                    char msg[64];
                    sprintf(msg, "%s: no such file or directory\n", cmd->command);
                    print_and_flush(msg);
                    set_status("exit value 1");
                    return;
                }
            }

            // exit status ?
            _exit(EXIT_FAILURE);
        }
        if (cmd->background == 0) {
            int stat;
            char status[32];
            waitpid(pid, &stat, 0);
            STATUS = WEXITSTATUS(stat);

            /* set status */
            sprintf(status, "exit value %d", STATUS);
            set_status(status);
        } else {
            printf("background pid is %ld\n", (long)pid);
        }
    }
}


int main(int argc, char *argv[])
{
    int exit_flag;
    char command[COMMAND_LENGTH];
    struct Command* current_command;

    exit_flag = 0;

    struct sigaction act;
    act.sa_handler = background_exit;
    act.sa_flags = SA_NODEFER;
    sigaction(SIGCHLD, &act, NULL);

    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    do {

        sa.sa_handler = SIG_IGN;
        if (sigaction(SIGINT, &sa, NULL) == -1) {
            perror("sigaction");
            exit(1);
        }

        get_input(command, COMMAND_LENGTH);


        sa.sa_handler = sigint_handler;
        if (sigaction(SIGINT, &sa, NULL) == -1) {
            perror("sigaction");
            exit(1);
        }

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

    exit(0);
}
