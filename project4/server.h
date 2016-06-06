#include <netinet/in.h>

#ifndef SERVER_H
#define SERVER_H

void start_server(int* sockfd, struct sockaddr_in* serv_addr);
void accept_new_connection(int* sockfd,
                           int* client_sockfd,
                           struct sockaddr_in* cli_addr);
int receive_text(int* client_sockfd, char* buffer, int buffer_size);
void send_text();

#endif  // SERVER_H
