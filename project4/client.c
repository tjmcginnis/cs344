#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void make_connection(int* sockfd, int portno)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;

    server = gethostbyname("localhost");
    if (server == NULL) {
        perror("no such server");
        exit(1);
    }
    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*) server->h_addr, (char*) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(*sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("error connecting");
        exit(1);
    }
}

void send_text();
void receive_text();
void write_text();
