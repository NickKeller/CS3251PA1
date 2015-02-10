#include "udp-client.h"
#include "md5.h"

int main(int argc, char *argv[]){
	
	if(argc < 2){
		print_use_and_exit();
	}
	
	char* port = argv[1];
	//create a socket
	CONN_INFO* connection = setup_socket("127.0.0.1",port);
	//created socket, now to make data and sendto my server
	char request[] = "REQ: Please Connect\n";
	int numBytesSent = sendto(connection->socket,request,sizeof(request),0,
							  connection->remote_addr,connection->addrlen);
	printf("Message size: %d, num bytes sent: %d\n",sizeof(request),numBytesSent);
	//wait for a response
	char recvBuffer[100];

	int numBytesRecv = numBytesRecv = recvfrom(connection->socket,recvBuffer,sizeof(recvBuffer),0,
								connection->remote_addr,&(connection->addrlen));

	printf("Received message: %s\n",recvBuffer);
	//compute md5 hash
	char * response = calloc(110,sizeof(char));
	doMD5(recvBuffer,response, "user1", "pass1");


	return 0;
}

void print_use_and_exit(){
	fprintf(stderr,"Usage:  client-udp port\n\n");
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


void doMD5(char* buffer, char* response, char* username, char* password){
	//concat the 3 strings
	unsigned char *temp = calloc(strlen(buffer)+strlen(username)+strlen(password),sizeof(char));
	strcat(temp,username);
	strcat(temp,buffer);
	strcat(temp,password);
	unsigned char output[16];
	md5(temp,strlen(temp),output);
}