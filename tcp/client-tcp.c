#include "client-tcp.h"

int DEBUG;
CONN_INFO* connection;

int main(int argc, char *argv[]){
	

	int opts;
	int offset = 0;
	if((opts = getopt(argc,argv,"d")) != -1){
		DEBUG = 1;
		offset = 1;
	}
	else{
		DEBUG = 0;
	}

	if(argc < (5+offset)){
		print_use_and_exit();
	}
	char* ip = argv[1+offset];
	char* port = argv[2+offset];
	char* username = argv[3+offset];
	char* password = argv[4+offset];
	//create a socket to connect on
	connection = setup_socket(ip,port);
	if(connect(connection->socket,connection->addr,connection->addrlen) < 0){
		perror("Cannot connect to server\n");
		return 0;
	}
	
	//send a connection request
	char request[] = "REQ: Please Connect\n";
	int numBytesSent = send(connection->socket,request,sizeof(request),0);
	if(DEBUG) printf("Message size: %d, num bytes sent: %d\n",sizeof(request),numBytesSent);

	//receive the challenge
	char challengeBuf[100];
	int numBytesRecv = recv(connection->socket,challengeBuf,sizeof(challengeBuf),0);
	if(DEBUG) printf("Bytes Received:%d\nReceived challenge: %s\n",numBytesRecv,challengeBuf);

	//received challenge, compute md5 hash
	char* space = strchr(challengeBuf,' ');
	int pos = space - challengeBuf;

	char* newLine = strchr(challengeBuf,'\n');
	int newLineSpace = newLine - challengeBuf;

	if(DEBUG) printf("Challenge string is:%s_blah\n",&challengeBuf[pos]);

	char* challenge = calloc(110,sizeof(char));
	memcpy(challenge,&challengeBuf[pos+1],newLineSpace-pos-1);

	if(DEBUG) printf("Filtered challenge string is:%s_blah\n",challenge);
	//now have the challenge string, now to compute the md5 hash
	char* md5Res = calloc(110,sizeof(char));
	md5Res = doMD5(challenge,username,password);

	//build the response buffer
	char* respHead = "RES: ";
	char* respBuffer = calloc(100,sizeof(char));

	strcat(respBuffer,respHead);
	strcat(respBuffer,username);
	strcat(respBuffer," ");
	strcat(respBuffer,md5Res);
	strcat(respBuffer,"\n");
	
	//send back the md5 hash
	numBytesSent = send(connection->socket,respBuffer,strlen(respBuffer),0);

	//get back a response
	char finalBuffer[100];
	numBytesRecv = recv(connection->socket,finalBuffer,sizeof(finalBuffer),0);

	printf("%s\n",&finalBuffer[5]);

	close(connection->socket);
}


CONN_INFO* setup_socket(char* host, char* port){
	//this code was from my CS2200 class, for the networking project we did
	struct addrinfo *connections, *conn = NULL;
	struct addrinfo info;
	memset(&info, 0, sizeof(struct addrinfo));
	int sock = 0;
	info.ai_family = AF_INET;
	info.ai_socktype = SOCK_STREAM;
	info.ai_protocol = IPPROTO_TCP;
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
	conn_info->addr = conn->ai_addr;
	conn_info->addrlen = conn->ai_addrlen;
	return conn_info;
}

void print_use_and_exit(){
	fprintf(stderr,"Usage:  client-tcp [-d] ip port username password\n\n"); 
	exit (EXIT_FAILURE);
}

char* doMD5(char* buffer, char* username, char* password){
	//concat the 3 strings
	unsigned char *temp = calloc(strlen(buffer)+strlen(username)+strlen(password),sizeof(char));
	strcat(temp,username);
	strcat(temp,buffer);
	strcat(temp,password);
	char* output = md5(temp,strlen(temp));
	if(DEBUG) printf("MD5 hash is:%s_blah\nLength is %d\nStrlen of temp:%d\n",output,strlen(output),strlen(temp));
	return output;
}