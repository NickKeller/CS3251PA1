#include "udp-server.h"


int main(int argc, char *argv[]){
	//must have one argument - the port number
	if(argc < 2){
		print_use_and_exit();
	}
	
	int port = argv[1];
	
	return 0;
}

void print_use_and_exit(){
	fprintf(stderr,"Usage:  server-udp port\n\n");
	exit (EXIT_FAILURE);
}
