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

#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H


typedef struct _CONN_INFO{
	int socket;
	socklen_t addrlen;
	struct sockaddr *remote_addr;
} CONN_INFO;

typedef struct _MESSAGE{
	char buffer[1024];
	int type;
} MESSAGE;

//function declarations
void print_use_and_exit(void);
CONN_INFO* setup_socket(char* host, char* port);


#endif//end udp-client.h
