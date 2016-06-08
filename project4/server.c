/*
 * Author: Tyler McGinnis
 * CS344
 * Program 4
 *
 * File: server.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


/*
 * Set up the serv_addr struct for a new server
 */
void setup_server(struct sockaddr_in* serv_addr, int portno)
{
    bzero((char*) serv_addr, sizeof(*serv_addr));
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_addr.s_addr = INADDR_ANY;
    serv_addr->sin_port = htons(portno);
}

/*
 * Start the server represented by serv_addr
 */
void start_server(int* sockfd, struct sockaddr_in* serv_addr)
{
    if (bind(*sockfd, (struct sockaddr*) serv_addr, sizeof(*serv_addr)) < 0) {
        perror("bind");
        exit(1);
    }
    listen(*sockfd, 5);
}

/*
 * Accept new connection from a client
 */
void accept_new_connection(int* sockfd,
                           int* client_sockfd,
                           struct sockaddr_in* cli_addr)
{
    socklen_t clilen = sizeof(*cli_addr);
    *client_sockfd = accept(*sockfd, (struct sockaddr*) cli_addr, &clilen);

    if (*client_sockfd < 0) {
        perror("accept");
        exit(1);
    }
}
