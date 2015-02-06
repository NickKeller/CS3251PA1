#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#ifndef UDP_SERVER_H
#define UDP_SERVER_H

enum {REQ=0, CHA, RESP, ACK};


void print_use_and_exit(void);


#endif//end udp-server.h
