/*
 * Author: Tyler McGinnis
 * CS344
 * Program 4
 *
 * File: utils.h
 */

#ifndef UTILS_H
#define UTILS_H

/*
 * Set up a new socket
 */
void setup_socket(int* sockfd);

/*
 * Receive text from a socket into a buffer
 */
int receive_text(int* sockfd, char* buffer, int buffer_size);

/*
 * Send text from a buffer through a socket
 */
int send_text(int* sockfd, char* buffer, int buffer_size);

/*
 * Reentrant safe printf
 */
void write_text(char* buffer, int buffer_size);

/*
 * Transmit a message of any size over a socket
 */
void transmit_message(int* sockfd, char* message);

/*
 * Receive a message of any size from a socket
 */
char* receive_message(int* sockfd);

/*
 * Get the length of a file (strips trailing newline)
 */
int get_file_length(char* file_name);

/*
 * Check that all chars in a file are uppercase letters or spaces
 */
int check_for_illegal_chars(char* file_name);

#endif  // UTILS_H
