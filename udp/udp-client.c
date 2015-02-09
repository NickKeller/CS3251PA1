#include "udp-client.h"

int main(int argc, char *argv[]){
	
	if(argc < 2){
		print_use_and_exit();
	}
	
	char* port = argv[1];
	//create a socket
	CONN_INFO* connection = setup_socket("127.0.0.1",port);
	//created socket, now to make data and sendto my server
	char text[] = "This is a test message for receiving";
	int msgSize = sizeof(MESSAGE);
	MESSAGE* message = calloc(1,msgSize);
	memcpy(message->buffer,text,sizeof(text));
	message->type = REQ;
	int numBytesSent = sendto(connection->socket,message,msgSize,0,
							  connection->remote_addr,connection->addrlen);
	printf("Message size: %d, num bytes sent: %d\n",msgSize,numBytesSent);
	return 0;
}

void print_use_and_exit(){
	fprintf(stderr,"Usage:  server-udp port\n\n");
	exit (EXIT_FAILURE);
}

CONN_INFO* setup_socket(char* host, char* port){
	//this code was from my CS2200 class, for the networking project we did
	struct addrinfo *connections, *conn = NULL;
	struct addrinfo info;
	memset(&info, 0, sizeof(struct addrinfo));
	int sock = 0;

	info.ai_family = AF_INET;
	info.ai_socktype = SOCK_DGRAM;
	info.ai_protocol = IPPROTO_UDP;
	getaddrinfo(host, port, &info, &connections);

	/*for loop to determine correct addr info*/ 
	for(conn = connections; conn != NULL; conn = conn->ai_next){
		sock = socket(conn->ai_family, conn->ai_socktype, conn->ai_protocol);
		if(sock <0){
			continue;
		}
		break;
	}
	if(conn == NULL){
		perror("Failed to find and bind a socket\n");
		return NULL;
	}
	CONN_INFO* conn_info = malloc(sizeof(CONN_INFO));
	conn_info->socket = sock;
	conn_info->remote_addr = conn->ai_addr;
	conn_info->addrlen = conn->ai_addrlen;
	return conn_info;
}
