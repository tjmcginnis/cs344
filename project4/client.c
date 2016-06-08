/*
 * Author: Tyler McGinnis
 * CS344
 * Program 4
 *
 * File: client.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/*
 * makes a connection to server named host on port number portno
 */
int make_connection(int* sockfd, const char* host, int portno)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;

    /* look up server identified by host */
    server = gethostbyname(host);
    if (server == NULL) {
        perror("no such server");
        exit(1);
    }

    /* built up serv_addr */
    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*) server->h_addr, (char*) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    /* attempt to connect, return 1 if unsuccessful */
    if (connect(*sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        return 1;
    }

    return 0;
}
