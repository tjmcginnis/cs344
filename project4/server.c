#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


void start_server(int* sockfd, struct sockaddr_in* serv_addr)
{
    if (bind(*sockfd, (struct sockaddr*) serv_addr, sizeof(*serv_addr)) < 0) {
        perror("bind");
        exit(1);
    }
    printf("Server listening\n");
    listen(*sockfd, 5);
}

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
    printf("Connection accepted from client\n");
}

void receive_text() {}
void send_text() {}
