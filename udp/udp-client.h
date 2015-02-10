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

enum{REQ=0,CHA,RESP,ACK,NACK};

typedef struct _CONN_INFO{
	int socket;
	socklen_t addrlen;
	struct sockaddr *remote_addr;
} CONN_INFO;

//function declarations
void print_use_and_exit(void);
CONN_INFO* setup_socket(char* host, char* port);
void doMD5(char* buffer, char* response, char* username,char* password);


#endif//end udp-client.h
