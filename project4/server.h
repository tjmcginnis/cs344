/*
 * Author: Tyler McGinnis
 * CS344
 * Program 4
 *
 * File: server.h
 */

#include <netinet/in.h>

#ifndef SERVER_H
#define SERVER_H

/*
 * Set up the serv_addr struct for a new server
 */
void setup_server(struct sockaddr_in* serv_addr, int portno);

/*
 * Start the server represented by serv_addr
 */
void start_server(int* sockfd, struct sockaddr_in* serv_addr);

/*
 * Accept new connection from a client
 */
void accept_new_connection(int* sockfd,
                           int* client_sockfd,
                           struct sockaddr_in* cli_addr);

#endif  // SERVER_H
