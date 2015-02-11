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

#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

typedef struct _CONN_INFO{
	int socket;
	socklen_t addrlen;
	struct sockaddr *addr;
} CONN_INFO;

//function declarations
void print_use_and_exit(void);
CONN_INFO* setup_socket(char* host, char* port);
char* doMD5(char* buffer,char* username,char* password);
unsigned *md5( const char *msg, int mlen);


#endif//end client-tcp.h
