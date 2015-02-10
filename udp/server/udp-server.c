#include "udp-server.h"
#include "md5.h"

//usernames and passwords
char *usernames[] = {"user1","user2","user3"};
char *passwords[] = {"pass1","pass2","pass3"};

CLIENT* client1;
CLIENT* client2;

char* REQ_MSG = "REQ";
char* RES_MSG = "RES";

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
	char buffer[BUFFER_SIZE];
	char *response = calloc(BUFFER_SIZE,sizeof(char));	
	//now, infinitely wait and process requests as they come in
	while(1){
		printf("-----------------Waiting to receive-----------------\n");
		int sizeReceived = 0;
		//allow a maximum of 1024 bytes for this login process, don't need more than that
		sizeReceived = recvfrom(sock,buffer,sizeof(buffer),0, (struct sockaddr*)&remaddr,&addrlen);
		printf("Message Received:%s\nMessage size:%d\nPort Received From:%d\nIP Received from: %s\n",buffer,sizeReceived,remaddr.sin_port,inet_ntoa(remaddr.sin_addr));
		//process the request
		if(process(buffer,sizeof(buffer),port,&response)){
			printf("Sending Response:\n%s\n",response);
			//send the response
			int numBytesSent = 0;
			if((numBytesSent = sendto(sock, response,BUFFER_SIZE,0,(struct sockaddr*)&remaddr,addrlen)) < 0){
				perror("Failed on sending challenge");
				return 0;
			}
		}		
		//zero out the recv and response buffers
		memset(buffer,0,BUFFER_SIZE);
		memset(response,0,BUFFER_SIZE);
		
	}
	return 0;
}

void print_use_and_exit(){
	fprintf(stderr,"Usage:  server-udp port\n\n");
	exit (EXIT_FAILURE);
}

int process(char* message,int sizeOfBuffer, int port, char ** response){
	//make sure there is a newline character, that signifies the end of a request
	char* buffer = message;
	int numChars = 0;
	int foundNewLine = 0;
	while(1){
		char* current = &buffer[numChars];
		if(numChars == sizeOfBuffer){
			break;
		}
		if(strcmp(current,"\n") == 0){
			foundNewLine = 1;
			numChars--;
			break;
		}
		numChars++;
	}
	if(foundNewLine){
		//valid response, can actually process it
		printf("Valid message, processing %d chars,\n",numChars);
		//first 3 chars are the type of request from a client(Either REQ or RES)
		if(strncmp(buffer,"REQ",3) == 0){
			return process_request(&buffer[5],numChars-5,response);
		}
		if(strncmp(buffer,"RES",3) == 0){
			return process_response(&buffer[5],numChars-5,response);
		}
		return 1;
	}
	else{
		return 0;
	}
}

int process_response(char *buffer, int sizeOfBuffer, char ** response){
	printf("Response to process is:%s\n",buffer);
	return 1;
}
int process_request(char *buffer, int sizeOfBuffer, char ** response){
	printf("Request to process is:%s\n",buffer);
	int result = 0;
	//has to also have the text "Please Connect\n" to be a valid request
	if(strncmp(buffer,"Please Connect\n",sizeOfBuffer) == 0){
		printf("Valid Request\n");
		char * genString = generate_string();
		printf("String returned was %s\n",genString);
		char *head = "CHA: ";
		strcat(*response,head);
		strcat(*response,genString);
		strcat(*response,"\n");
		result = 1;
	}
	return result;
}

char * generate_string(){
	//random generation code modified from: http://stackoverflow.com/questions/15767691/whats-the-c-library-function-to-generate-random-string
	char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int length = 64;
    char * result = calloc(length,sizeof(char));
    srand(time(NULL));
    char * dest = result;
    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
	return result;
}