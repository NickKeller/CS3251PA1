#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#define BUFFER_SIZE 100

typedef struct _CLIENT{
	int port;
	char* ip;
	char challenge[64];
} CLIENT;

//function declarations
void print_use_and_exit(void);
int process(char* buffer, int sizeOfBuffer, int port,char ** response);
int process_response(char *buffer, int sizeOfBuffer, char ** response);
int process_request(char *buffer, int sizeOfBuffer, char ** response);
char * generate_string(void);
char* doMD5(char* buffer, char* username,char* password);
unsigned *md5( const char *msg, int mlen);
void figureOutClient(struct sockaddr_in remaddr);
//CONN_INFO* setup_socket(char* port);


#endif//end server-tcp.h
