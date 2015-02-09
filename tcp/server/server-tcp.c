#include "server-tcp.h"

int main(int argc, char* argv[]){
	
	//have to include the port number
	if(argc < 2){
		print_use_and_exit();
	}
	
	int port = atoi(argv[1]);
	
	//set up a tcp socket, and allow it to be reused
	int sockoptval;
	int sock;
	if((sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0){
		perror("Cannot create socket\n");
		return 0;
	}
	//allows reuse of the socket
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));
	//bind to the port
	printf("Binding to port %d\n",port);
	struct sockaddr_in myaddr;
	socklen_t addrlen = sizeof(myaddr);
	memset(&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(port);
	int bindResult = bind(sock, (struct sockaddr *)&myaddr,addrlen);
	if(bindResult == 0){
		printf("Ready to receive on port %d\n",port);
	}
	else{
		perror("Binding failed\n");
		return 0;
	}
	//now, listen for incoming connections, and allow up to 2
	printf("Listening for connections\n");
	if(listen(sock,2) < 0){
		perror("Listen failed\n");
		return 0;
	}
	
	//ready for connections
	while(1){
		struct sockaddr_in recvaddr;
		int recvAddrLen = sizeof(recvaddr);
		printf("Accepting an available connection\n");
		int recvSock = accept(sock,(struct sockaddr *)&recvaddr,&recvAddrLen);
		if(recvSock < 0){
			perror("Accept Failed");
			exit(0);
		}
		printf("Connection accepted\n");
		//ready to receive from the socket
		MESSAGE *message = calloc(1,sizeof(MESSAGE));
		int numBytesReceived = 0;
		printf("----------------------Waiting for Message----------------------\n");
		while((numBytesReceived += recv(recvSock,message,sizeof(MESSAGE),0)) < sizeof(MESSAGE));
		if(numBytesReceived == -1){
			perror("Recv failed\n");
			exit(0);
		}
		printf("Num Bytes Received:%d\nMessage Received is:%s\n",
				numBytesReceived,message->buffer);		
	}
	
	return 0;
}

void print_use_and_exit(){
	fprintf(stderr,"Usage:  server-tcp port\n\n");
	exit (EXIT_FAILURE);
}
