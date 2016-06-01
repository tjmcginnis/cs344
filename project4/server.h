#include <netinet/in.h>

#ifndef SERVER_H
#define SERVER_H

void start_server(int* sockfd, struct sockaddr_in* serv_addr);
void accept_new_connection(int* sockfd,
                           int* client_sockfd,
                           struct sockaddr_in* cli_addr);
void receive_text();
void send_text();

#endif  // SERVER_H
