/*
 * Author: Tyler McGinnis
 * CS344
 * Program 4
 *
 * File: utils.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#include <assert.h>
#include <sys/socket.h>

#define MAX_BUFFER_SIZE 1000

/*
 * Receive text from a socket into a buffer
 */
int receive_text(int* sockfd, char* buffer, int buffer_size)
{
    int n;

    memset(buffer, 0, buffer_size);
    n = read(*sockfd, buffer, buffer_size);
    if (n < 0) {
        perror("read");
    }

    return n;
}

/*
 * Send text from a buffer through a socket
 */
int send_text(int* sockfd, char* buffer, int buffer_size)
{
    int n;
    n = write(*sockfd, buffer, buffer_size);
    if (n < 0) {
        perror("write");
    }

    return n;
}

/*
 * Reentrant safe printf
 */
void write_text(char* buffer, int buffer_size)
{
    write(STDOUT_FILENO, buffer, buffer_size);
    fflush(stdout);
}

/*
 * Set up a new socket
 */
void setup_socket(int* sockfd)
{
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("sockfd");
        exit(1);
    }
}

/*
 * Transmit a message of any size over a socket
 */
void transmit_message(int* sockfd, char* message)
{
    int total_sent, bytes_sent, bytes_rcvd, message_length;
    char rcv_buffer[MAX_BUFFER_SIZE];
    char send_buffer[MAX_BUFFER_SIZE];

    /* initialize variables */
    total_sent = 0;
    bytes_sent = 0;
    bytes_rcvd = 0;
    message_length = strlen(message) + 1;
    memset(rcv_buffer, 0, MAX_BUFFER_SIZE);
    memset(send_buffer, 0, MAX_BUFFER_SIZE);

    /* send total message size to server */
    snprintf(send_buffer, MAX_BUFFER_SIZE, "%d", message_length);
    send_text(sockfd, send_buffer, MAX_BUFFER_SIZE);

    do {
        /* send part of message and store number of bytes sent */
        memset(send_buffer, 0, MAX_BUFFER_SIZE);
        memcpy(send_buffer, message + total_sent, MAX_BUFFER_SIZE - 1);
        bytes_sent = send_text(sockfd, send_buffer, MAX_BUFFER_SIZE) - 1;

        /* receive number of bytes received (by server) */
        memset(rcv_buffer, 0, MAX_BUFFER_SIZE);
        receive_text(sockfd, rcv_buffer, MAX_BUFFER_SIZE);
        bytes_rcvd = atoi(rcv_buffer) - 1;

        /* if good transmission, send "ok" back to server */
        if (bytes_rcvd == bytes_sent) {
            total_sent = total_sent + bytes_sent;
            memset(send_buffer, 0, MAX_BUFFER_SIZE);
            memcpy(send_buffer, "ok", strlen("ok"));
            send_text(sockfd, send_buffer, MAX_BUFFER_SIZE);
        }
    } while (total_sent < message_length);
}

/*
 * Receive a message of any size from a socket
 */
char* receive_message(int* sockfd)
{
    int total_rcvd, bytes_rcvd, message_length;
    char* message;
    char rcv_buffer1[MAX_BUFFER_SIZE];
    char rcv_buffer2[MAX_BUFFER_SIZE];
    char send_buffer[MAX_BUFFER_SIZE];

    /* initialize variables */
    total_rcvd = 0;
    bytes_rcvd = 0;
    memset(rcv_buffer1, 0, MAX_BUFFER_SIZE);
    memset(rcv_buffer2, 0, MAX_BUFFER_SIZE);
    memset(send_buffer, 0, MAX_BUFFER_SIZE);

    /* receive total message size from client */
    receive_text(sockfd, rcv_buffer1, MAX_BUFFER_SIZE);
    message_length = atoi(rcv_buffer1);
    message = calloc(message_length + 1, sizeof(char*));
    memset(message, 0, message_length);

    do {
        /* receive part of message and store number of bytes received */
        memset(rcv_buffer1, 0, MAX_BUFFER_SIZE);
        bytes_rcvd = receive_text(sockfd, rcv_buffer1, MAX_BUFFER_SIZE);

        /* send number of bytes received back to client */
        memset(send_buffer, 0, MAX_BUFFER_SIZE);
        snprintf(send_buffer, MAX_BUFFER_SIZE, "%d", bytes_rcvd);
        send_text(sockfd, send_buffer, MAX_BUFFER_SIZE);

        /* receive confirmation of successful transmission from client */
        memset(rcv_buffer2, 0, MAX_BUFFER_SIZE);
        receive_text(sockfd, rcv_buffer2, MAX_BUFFER_SIZE);
        if (strncmp(rcv_buffer2, "ok", strlen("ok")) == 0) {
            /* concatenate received text to message */
            strncat(message, rcv_buffer1, strlen(rcv_buffer1) + 1);

            total_rcvd = total_rcvd + bytes_rcvd;
        }
    } while (total_rcvd < message_length);

    return message;
}

/*
 * Get the length of a file (strips trailing newline)
 */
int get_file_length(char* file_name)
{
    int length;
    FILE *fp = fopen(file_name, "r");
    fseek(fp, 0, SEEK_END);
    length = ftell(fp) - 1;
    rewind(fp);
    fclose(fp);
    return length;
}

/*
 * Check that all chars in a file are uppercase letters or spaces
 */
int check_for_illegal_chars(char* file_name)
{
    FILE *fp;
    char* text;
    int i, stat, size, ret;
    regex_t regex;

    ret = 0;

    /* compile regex */
    stat = regcomp(&regex, "[^A-Z ]", REG_NOSUB);
    assert(stat == 0);

    size = get_file_length(file_name);

    fp = fopen(file_name, "r");
    text = calloc(size + 1, sizeof(char*));
    assert(text != NULL);
    fread(text, 1, size, fp);
    fclose(fp);

    /* check for matches */
    stat = regexec(&regex, text, 0, NULL, 0);
    if (stat == 0) {
        /* if match, set return value to 1 */
        ret = 1;
    }

    free(text);
    return ret;
}
