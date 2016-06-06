#ifndef CLIENT_H
#define CLIENT_H

void make_connection(int* sockfd, int portno);
int send_text(int* sockfd, char* message);
void receive_text();
void write_text();

#endif
