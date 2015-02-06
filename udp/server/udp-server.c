#include "udp-server.h"


int main(int argc, char *argv[]){
	//must have one argument - the port number
	if(argc < 2){
		print_use_and_exit();
	}
	
	char* port = argv[1];
	//create a socket to bind on
	CONN_INFO* connection = setup_socket(port);
	printf("Binding server on port %s\n",port);
	int bindResult = bind(connection->socket,connection->remote_addr,connection->addrlen);
	if(bindResult == 0){
		printf("Ready to receive on port %s\n",port);
	}
	else{
		printf("Binding failed\n");
	}
	return 0;
}

void print_use_and_exit(){
	fprintf(stderr,"Usage:  server-udp port\n\n");
	exit (EXIT_FAILURE);
}

CONN_INFO* setup_socket(char* port){
	struct addrinfo *connections, *conn = NULL;
	struct addrinfo info;
	memset(&info, 0, sizeof(struct addrinfo));
	int sock = 0;
	
	info.ai_family = AF_INET;
	info.ai_socktype = SOCK_DGRAM;
	info.ai_protocol = IPPROTO_UDP;
	getaddrinfo("127.0.0.1",port,&info,&connections);
	for(conn = connections; conn != NULL; conn = conn->ai_next){
		sock =  socket(conn->ai_family, conn->ai_socktype, conn->ai_protocol);
		if(sock < 0){
			printf("Failed to create socket\n");
			continue;
		}
		break;
	}
	if(conn == NULL){
		printf("Failed to create socket\n");
	}
	CONN_INFO* conn_info = malloc(sizeof(CONN_INFO));
	conn_info->socket = sock;
	conn_info->remote_addr = conn->ai_addr;
	conn_info->addrlen = conn->ai_addrlen;
	return conn_info;	
}
