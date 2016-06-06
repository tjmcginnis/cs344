#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "server.h"

int main(int argc, char* argv[])
{
    int sockfd, newsockfd, portno;
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[1000];

    if (argc < 2) {
        perror("port");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("sockfd");
        exit(1);
    }

    bzero((char*) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    start_server(&sockfd, &serv_addr);

    while (1) {
        accept_new_connection(&sockfd, &newsockfd, &cli_addr);
        receive_text(&newsockfd, buffer, 1000);
        printf("Message: %s\n", buffer);
    }

    close(newsockfd);
    close(sockfd);

    return 0;
}
