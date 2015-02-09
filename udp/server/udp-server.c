#include "udp-server.h"

//usernames and passwords
char *usernames[] = {"user1","user2","user3"};
char *passwords[] = {"pass1","pass2","pass3"};

CLIENT* client1;
CLIENT* client2;


int main(int argc, char *argv[]){
	//must have one argument - the port number
	if(argc < 2){
		print_use_and_exit();
	}
	
	//initialize the clients
	client1 = calloc(1,sizeof(CLIENT));
	client2 = calloc(1,sizeof(CLIENT));
	
	int port = atoi(argv[1]);
	//create a socket to bind on
	//I got this from: https://www.cs.rutgers.edu/~pxk/417/notes/sockets/udp.html


	int sock;
	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("Cannot create socket\n");
		return 0;
	}
	//CONN_INFO* connection = setup_socket(port);
	printf("Binding server on port %d\n",port);
	struct sockaddr_in myaddr;
	struct sockaddr_in remaddr;
	socklen_t addrlen = sizeof(remaddr);
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(port);
	int bindResult = bind(sock, (struct sockaddr *)&myaddr, sizeof(myaddr));
	if(bindResult == 0){
		printf("Ready to receive on port %d\n",port);
	}
	else{
		printf("Binding failed\n");
	}
	//only allocate for the buffers once, and just zero it out when done processing
	MESSAGE* recvBuffer = calloc(1,sizeof(MESSAGE));
	
	//now, infinitely wait and process requests as they come in
	while(1){
		printf("-----------------Waiting to receive-----------------\n");
		
		int sizeReceived = 0;
		char buffer[1024];
		char *current = &buffer[0];
		while(*current != '\n'){
			sizeReceived += recvfrom(sock,current,1,0,
									(struct sockaddr*)&remaddr,&addrlen);
			printf("Message Received:%s\nMessage size:%d\nPort Received From:%d\n",
					buffer,sizeReceived,remaddr.sin_port);
			current++;
		}
		//process the request
		//MESSAGE* response = process(recvBuffer,port);
		//send the response
		int numBytesSent = 0;
		//if((numBytesSent = sendto(sock, 
		
		//zero out the recv and response buffers
		memset(recvBuffer,0,sizeof(MESSAGE));
		
	}
	return 0;
}

void print_use_and_exit(){
	fprintf(stderr,"Usage:  server-udp port\n\n");
	exit (EXIT_FAILURE);
}

/*CONN_INFO* setup_socket(char* port){
	//I got this from: https://www.cs.rutgers.edu/~pxk/417/notes/sockets/udp.html
	struct sockaddr_in myaddr;
	int sock;
	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("Cannot create socket\n");
	}
	CONN_INFO* conn_info = malloc(sizeof(CONN_INFO));
	conn_info->socket = sock;
	conn_info->addr = INADDR_ANY;
	conn_info->addrlen = sizeof(myaddr);
	//print some information
	return conn_info;	
}*/
