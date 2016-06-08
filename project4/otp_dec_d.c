/*
 * Author: Tyler McGinnis
 * CS 344
 * Program 4
 *
 * File: otp_dec_d.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <signal.h>
#include <assert.h>

#include "server.h"
#include "utils.h"
#include "crypto.h"

#define MAX_SERVERS 5

int NUM_SERVERS = 0;
pid_t SERVERS[MAX_SERVERS];

void reap_background_processes();
void fork_new_server(int portno);

int main(int argc, char* argv[])
{
    int sockfd, newsockfd, portno, port_increment, bytes_rcvd;
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[1000], port[32];
    int new_port;

    port_increment = 10;  // used to determine port for forked servers

    if (argc < 2) {
        perror("port");
        exit(1);
    }

    /* don't leave zombie processes */
    signal(SIGCHLD, SIG_IGN);

    /* set up socket and start parent server */
    setup_socket(&sockfd);
    portno = atoi(argv[1]);
    setup_server(&serv_addr, portno);
    start_server(&sockfd, &serv_addr);

    while (1) {
        /* check for finished processes */
        reap_background_processes();
        memset(buffer, 0, 1000);

        /* accept connection */
        accept_new_connection(&sockfd, &newsockfd, &cli_addr);

        /* receive program name from client */
        bytes_rcvd = receive_text(&newsockfd, buffer, 1000);

        /* send program name to client */
        send_text(&newsockfd, "otp_dec_d", 9);

        /* make sure program is otp_enc */
        if (strncmp(buffer, "otp_dec", 7) == 0) {
            new_port = portno + port_increment;
            port_increment = port_increment + 10;
            memset(port, 0, 32);
            snprintf(port, 32, "%d", new_port);

            /* fork new server on new_port */
            fork_new_server(new_port);

            /* send new_port to otp_enc */
            send_text(&newsockfd, port, 32);
        }
    }

    close(newsockfd);
    close(sockfd);

    return 0;
}

/*
 * check for finished background processes and take
 * appropriate actions
 */
void reap_background_processes()
{
    int i, stat, latest;
    for (i = 0; i < NUM_SERVERS; i++) {
        latest = waitpid(-1, &stat, WNOHANG);
        if (SERVERS[i] == latest) {
            SERVERS[i] = 0;
            NUM_SERVERS = NUM_SERVERS - 1;
        }
    }
}

/*
 * fork a new process and start a server on portno
 */
void fork_new_server(int portno)
{
    pid_t pid;

    if (NUM_SERVERS < MAX_SERVERS) {
        NUM_SERVERS = NUM_SERVERS + 1;
        pid = fork();
    } else {
        perror("too many servers forked");
        return;
    }

    if (pid == -1) {
        perror("error forking new server");
    } else if (pid == 0) {
        int sockfd, client_sock;
        struct sockaddr_in serv_addr, cli_addr;
        char* plaintext;
        char* cipher;
        char* key;

        setup_socket(&sockfd);

        setup_server(&serv_addr, portno);
        start_server(&sockfd, &serv_addr);

        accept_new_connection(&sockfd, &client_sock, &cli_addr);

        /* receive cipher */
        cipher = receive_message(&client_sock);

        /* receive key */
        key = receive_message(&client_sock);

        /* make plaintext */
        plaintext = decrypt_cipher(cipher, key);

        /* send cipher */
        transmit_message(&client_sock, plaintext);

        free(plaintext);
        free(cipher);
        free(key);

        /* kill child process */
        kill(getpid(), SIGTERM);
    }

    SERVERS[NUM_SERVERS] = pid;
}
