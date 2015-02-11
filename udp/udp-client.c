#include "udp-client.h"

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
	//create a socket
	connection = setup_socket(ip,port);
	//created socket, now to make data and sendto my server
	char request[] = "REQ: Please Connect\n";
	
	int retval = -1;

	int numBytesSent = sendto(connection->socket,request,sizeof(request),0,
							  connection->remote_addr,connection->addrlen);
	if(DEBUG) printf("Message size: %d, num bytes sent: %d\n",sizeof(request),numBytesSent);
	//wait for a response
	char recvBuffer[100];
	int size = sizeof(recvBuffer);
	int numBytesRecv = recvfrom(connection->socket,recvBuffer,size,0,
								connection->remote_addr,&(connection->addrlen));

	if(DEBUG) printf("Received message: %s\n",recvBuffer);
	//compute md5 hash
	//find the first space
	char* space = strchr(recvBuffer,' ');
	int pos = space - recvBuffer;

	char* newLine = strchr(recvBuffer,'\n');
	int newLineSpace = newLine - recvBuffer;

	if(DEBUG) printf("Challenge string is:%s_blah\n",&recvBuffer[pos]);

	char* challenge = calloc(110,sizeof(char));
	memcpy(challenge,&recvBuffer[pos+1],newLineSpace-pos-1);

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
	if(DEBUG) printf("Sending Response:\n%s_blah\n",respBuffer);
	//send off the response
	numBytesSent = sendto(connection->socket,respBuffer,strlen(respBuffer),0,
							connection->remote_addr,connection->addrlen);
	//wait for a response
	memset(recvBuffer,0,100);
	numBytesRecv = recvfrom(connection->socket,recvBuffer,sizeof(recvBuffer),0,
								connection->remote_addr,&(connection->addrlen));

	printf("%s\n",&recvBuffer[5]);


	return 0;
}

void print_use_and_exit(){
	fprintf(stderr,"Usage:  client-udp [-d] ip port username password\n\n"); 
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


int timeout_recvfrom (int sock, char *buf, int *length, struct sockaddr_in *connection, int timeoutinseconds)
{
    fd_set socks;
    struct timeval t;
    FD_ZERO(&socks);
    FD_SET(sock, &socks);
    t.tv_sec = timeoutinseconds;
    printf("Starting Select\n");
    select(sock + 1, &socks, NULL, NULL, &t);
    printf("Done with select\n");
    if (recvfrom(sock, buf, *length, 0, (struct sockaddr *)connection, length)!=-1)
        {
        return 1;
        }
    else
        return 0;
}