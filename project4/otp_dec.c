/*
 * Author: Tyler McGinnis
 * CS 344
 * Program 4
 *
 * File: otp_dec.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <assert.h>
#include <regex.h>

#include "client.h"
#include "utils.h"
#include "crypto.h"

char* PROG_NAME = "otp_dec";

int main(int argc, char* argv[])
{
    int sockfd, portno, cipher_size, key_size, stat;
    char buffer[256];
    char* output_buffer;
    char* plaintext;
    char* cipher;
    char* key;
    FILE* cipher_fp;
    FILE* key_fp;

    if (argc < 4) {
        perror("argc");
        exit(1);
    }

    cipher_size = get_file_length(argv[1]);
    key_size = get_file_length(argv[2]);

    /* check that input only contains legal chars */
    if (check_for_illegal_chars(argv[1])) {
        fprintf(stderr, "otp_dec error: input contains bad characters\n");
        exit(1);
    }

    /* check that key only contains legal chars */
    if (check_for_illegal_chars(argv[2])) {
        fprintf(stderr, "otp_dec error: key contains bad characters\n");
        exit(1);
    }

    /* check that key is at least the size of cipher input */
    if (key_size < cipher_size) {
        fprintf(stderr, "Error: key '%s' is too short\n", argv[2]);
        exit(1);
    }

    /* set up socket and attempt to connect to otp_dec_d */
    portno = atoi(argv[3]);
    setup_socket(&sockfd);
    stat = make_connection(&sockfd, "localhost", portno);
    if (stat == 1) {
        fprintf(stderr, "Error: could not contact otp_dec_d on port %d\n", portno);
        exit(2);
    }

    /* send program name to otp_enc_d in order to handshake */
    send_text(&sockfd, PROG_NAME, strlen(PROG_NAME));

    /* receive server program name and check it is correct */
    memset(buffer, 0, 256);
    receive_text(&sockfd, buffer, 256);
    if (strncmp(buffer, "otp_dec_d", 9) != 0) {
        fprintf(stderr, "Error: connected to program that is not otp_dec_d\n");
        close(sockfd);
        exit(1);
    }

    /* receive new port no to connect on from otp_enc_d */
    memset(buffer, 0, 256);
    receive_text(&sockfd, buffer, 256);
    portno = atoi(buffer);
    close(sockfd);  // close connection to original server process

    /* set up connection to new server process */
    setup_socket(&sockfd);
    stat = make_connection(&sockfd, "localhost", portno);
    if (stat == 1) {
        fprintf(stderr, "Error: could not contact otp_dec_d on port %d\n", portno);
        exit(2);
    }

    /* store cipher in variable */
    cipher_fp = fopen(argv[1], "r");
    cipher = calloc(cipher_size + 1, sizeof(char*));
    assert(cipher != NULL);
    fread(cipher, 1, cipher_size, cipher_fp);;
    fclose(cipher_fp);

    /* send cipher */
    transmit_message(&sockfd, cipher);
    free(cipher);

    /* store key in variable */
    key_fp = fopen(argv[2], "r");
    key = calloc(key_size + 1, sizeof(char*));
    assert(key != NULL);
    fread(key, 1, key_size, key_fp);
    fclose(key_fp);

    /* send key */
    transmit_message(&sockfd, key);
    free(key);

    /* receive plaintext */
    plaintext = receive_message(&sockfd);

    /* write plaintext to stdout */
    printf("%s\n", plaintext);

    free(plaintext);
    close(sockfd);
}
