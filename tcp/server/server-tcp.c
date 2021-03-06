#include "server-tcp.h"

//usernames and passwords
char *usernames[] = {"user1","user2","user123"};
char *passwords[] = {"pass1","pass2","pass3"};

int DEBUG;
char *challenge;

int main(int argc, char *argv[]){
	
	int offset = 0;
	if(getopt(argc,argv,"d") != -1){
		DEBUG = 1;
		offset = 1;
	}
	else{
		DEBUG = 0;
	}

	//must have one argument - the port number
	if(argc < (2+offset)){
		print_use_and_exit();
	}
	
	challenge = calloc(64,sizeof(char));
	int port = atoi(argv[1+offset]);
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
	if(DEBUG) printf("Listening for connections\n");
	if(listen(sock,2) < 0){
		perror("Listen failed\n");
		return 0;
	}
	
	//ready for connections
	while(1){
		struct sockaddr_in recvaddr;
		int recvAddrLen = sizeof(recvaddr);
		if(DEBUG) printf("Accepting an available connection\n");
		int recvSock = accept(sock,(struct sockaddr *)&recvaddr,&recvAddrLen);
		if(recvSock < 0){
			if(DEBUG) perror("Accept Failed");
			continue;
		}
		if(DEBUG) printf("Connection accepted\n");
		//ready to receive from the socket
		char* buffer = calloc(BUFFER_SIZE,sizeof(char));
		char* response = calloc(BUFFER_SIZE,sizeof(char));
		int numBytesReceived = 0;
		if(DEBUG) printf("----------------------Waiting for Message----------------------\n");
		numBytesReceived = recv(recvSock,buffer,BUFFER_SIZE,0);
		if(numBytesReceived == -1){
			perror("Recv failed\n");
		}
		if(DEBUG) printf("Num Bytes Received:%d\nMessage Received is:%s\n",
				numBytesReceived,buffer);
		if(process(buffer,sizeof(buffer),port,&response)){
			if(DEBUG) printf("Sending Response:\n%s\n",response);
			int numBytesSent = send(recvSock,response,BUFFER_SIZE,0);
			if(DEBUG) printf("Num Bytes Sent:%d\n",numBytesSent);
		}
		if(DEBUG) printf("----------------------Waiting for Message----------------------\n");
		numBytesReceived = recv(recvSock,buffer,BUFFER_SIZE,0);
		if(numBytesReceived == -1){
			perror("Recv failed\n");
		}
		if(DEBUG) printf("Num Bytes Received:%d\nMessage Received is:%s\n",numBytesReceived,buffer);
		if(process(buffer,sizeof(buffer),port,&response)){
			if(DEBUG) printf("Sending Response:\n%s\n",response);
			int numBytesSent = send(recvSock,response,BUFFER_SIZE,0);
			if(DEBUG) printf("Num Bytes Sent:%d\n",numBytesSent);
		}
		close(recvSock);
	}
	
	return 0;
}

void print_use_and_exit(){
	fprintf(stderr,"Usage:  server-tcp port\n\n");
	exit (EXIT_FAILURE);
}

int process(char* message,int sizeOfBuffer, int port, char ** response){
	//make sure there is a newline character, that signifies the end of a request
	if(DEBUG) printf("Starting messsage processing\n");
	//find the newLine
	char* newLine = strchr(message,'\n');
	if(newLine != NULL){
		//valid response, can actually process it
		int posOfNewLine = newLine - message;
		char* buffer = calloc(posOfNewLine,sizeof(char));
		memcpy(buffer,message,posOfNewLine);
		if(DEBUG) printf("Valid message, processing:%s_blah\n",buffer);
		//first 3 chars are the type of request from a client(Either REQ or RES)
		if(strncmp(buffer,"REQ",3) == 0){
			return process_request(&buffer[5],posOfNewLine,response);
		}
		if(strncmp(buffer,"RES",3) == 0){
			return process_response(&buffer[5],posOfNewLine,response);
		}
		return 1;
	}
	else{
		return 0;
	}
}

int process_response(char *buffer, int sizeOfBuffer, char ** response){
	if(DEBUG) printf("Response to process is:%s\n",buffer);
	int result = 0;
	//grab the username by finding the first space
	char* space;
	if(DEBUG) printf("Finding space\n");
	if((space = strchr(buffer,' ')) != NULL){
		//username is 0 to pos
		//MD5 is pos+1 to sizeOfBuffer
		//find the appropriate password
		int pos = space - buffer;
		char* username = calloc(pos,sizeof(char));
		memcpy(username,buffer,pos);
		if(DEBUG) printf("Username is:%s_blah\n",username);
		char* password;
		int foundUserName = 0;
		for(int i = 0; i < 3;i++){
			if(strcmp(usernames[i],username) == 0){
				password = passwords[i];
				if(DEBUG) printf("Password is %s_blah\n",password);
				if(DEBUG) printf("Challenge is %s_blah\n",challenge);
				foundUserName = 1;
				break;
			}
		}
		if(foundUserName){
			buffer+=(pos+1);
			//compare the user's md5 hash with my md5 hash
			//int size = strlen(challenge) + strlen(username) + strlen(password);
			char* md5 = calloc(110,sizeof(char));
			md5 = doMD5(challenge,username,password);

			if(DEBUG) printf("Doing comparison:\nCalculated: %s_blah\nReceived:   %s_blah\n",md5,buffer);
			int cmpRes = strncmp(md5,buffer,strlen(md5));
			if(DEBUG) printf("Done with comparison, result is %d\n",cmpRes);
			if(cmpRes == 0){
				//have a match!
				result = 1;
				*response = "ACK: Welcome to our service\n";
			}
			else{
				result = 1;
				*response = "NAK: Wrong Credentials\n";
				if(DEBUG) printf("Set response to %s\n",*response);
			}
		}
		else{
			*response = "NAK: Wrong Credentials\n";
		}
	}

	return 1;
}
int process_request(char *buffer, int sizeOfBuffer, char ** response){
	if(DEBUG) printf("Request to process is:%s\n",buffer);
	int result = 0;
	//has to also have the text "Please Connect" to be a valid request
	if(strncmp(buffer,"Please Connect",sizeOfBuffer) == 0){
		if(DEBUG) printf("Valid Request\n");
		char * genString = generate_string();
		if(DEBUG) printf("String returned was %s\n",genString);
		//memcpy(current_client->challenge,genString,64);
		challenge = genString;
		//test MD5
		//char* test = calloc(110,sizeof(char));
		//test = doMD5(genString,"user1","pass1");
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

char* doMD5(char* buffer, char* username, char* password){
	//md5 code gotten from http://rosettacode.org/wiki/MD5#C
	//concat the 3 strings
	unsigned char *temp = calloc(strlen(buffer)+strlen(username)+strlen(password),sizeof(char));
	strcat(temp,username);
	strcat(temp,buffer);
	strcat(temp,password);
	char* output = md5(temp,strlen(temp));
	//have to do this, because for some reason 3 extra bytes get added
	char* value = calloc(16,sizeof(char));
	memcpy(value,output,16);
	if(DEBUG) printf("MD5 hash is:%s_blah\nLength is %d\nStrlen of temp:%d\n",value,strlen(value),strlen(temp));
	return value;
}

