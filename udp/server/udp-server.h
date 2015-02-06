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

#ifndef UDP_SERVER_H
#define UDP_SERVER_H

enum {REQ=0, CHA, RESP, ACK};

typedef struct _CONN_INFO{
	int socket;
	socklen_t addrlen;
	struct sockaddr *remote_addr;
} CONN_INFO;

//function declarations
void print_use_and_exit(void);
CONN_INFO* setup_socket(char* port);


#endif//end udp-server.h
