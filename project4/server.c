#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int receive_text(int* client_sockfd, char* buffer, int buffer_size)
{
    int n;

    bzero(buffer, buffer_size);
    n = read(*client_sockfd, buffer, buffer_size);
    if (n < 0) {
        perror("read");
    }

    return n;
}

void send_text() {}
