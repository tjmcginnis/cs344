/*
 * Author: Tyler McGinnis
 * CS344
 * Program 4
 *
 * File: client.h
 */


#ifndef CLIENT_H
#define CLIENT_H

/*
 * makes a connection to server named host on port number portno
 */
int make_connection(int* sockfd, const char* host, int portno);

#endif  // CLIENT_H
