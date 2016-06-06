#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "client.h"

int main(int argc, char* argv[])
{
    int sockfd, portno;
    char* message;

    if (argc < 2) {
        perror("port");
        exit(1);
    }

    portno = atoi(argv[1]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("sockfd");
        exit(1);
    }

    make_connection(&sockfd, portno);

    message = "Testing... does this work\n";
    send_text(&sockfd, message);

    close(sockfd);
}
