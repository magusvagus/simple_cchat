#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h> // for strerrno()
#include <errno.h> // for error handling

#include <arpa/inet.h> // for inet_addr function
#include <sys/types.h> // for definition of socket functions
#include <sys/socket.h> // <- most important for sockets

#include <netinet/in.h> // structures to store adress information header file.

#include "functions.h"

int main(void){

	// create socket
	int clientSocket = 
		create_TCP_socket();

	// set server address
	struct sockaddr_in *serverAddress = 
		create_IPV4_address(9002, "127.0.0.1");

	if (serverAddress == NULL) {
		fprintf(stderr, "[ ER ] ERROR, memory allocation failed\n");
		printf("\tErrno: %d\n",errno);
		printf("\tErrmsg: %s\n",strerror(errno));
		return -1;
	}

	// connect to server
	int connectStatus;
	connectStatus = 
		connect(clientSocket, (struct sockaddr*) serverAddress, sizeof(*serverAddress));

	if(clientSocket == -1){
		printf("[ ER ] ERROR, connection failed.\n");
		printf("\tErrno: %d\n",errno);
		printf("\tErrmsg: %s\n",strerror(errno));
	}
	else{
		printf("[ OK ] Socket connection established.\n");
	}

	// server greeting page
	char serverResponse[1024];
	recv(clientSocket, &serverResponse, sizeof(serverResponse), 0);
	printf("\nServer: %s",serverResponse);

	// recieve nick request
	char serverNick[1024];
	recv(clientSocket, &serverNick, sizeof(serverNick), 0);
	printf("Server: %s\n",serverNick);

	// check nick
	printf("Enter nickname: ");
	char checkNick[22];
	fgets(checkNick, sizeof(checkNick), stdin);
	//checkNick[strlen(checkNick)-1]='\0';

	// add check for nicks longer than 10 char
	while(strlen(checkNick) == 0){
		printf("Invalid input, enter nickname: ");
		fgets(checkNick, sizeof(checkNick), stdin);
		checkNick[strlen(checkNick)-1]='\0';
	}
	send(clientSocket, checkNick, strlen(checkNick),0);

	// message loop start
	int loop = 1;
	while(loop){
		char clientMessage[1024];
		printf("> ");
		fgets(clientMessage, sizeof(clientMessage), stdin);

		// we need \n at the end for the server to display
		// so we need to add it to the str we are comparing to
		if(strcmp(clientMessage, "/quit\n") !=0){
		send(clientSocket, clientMessage, strlen(clientMessage),0);
		}
		else{
			loop = 0;
			printf("Closing.\n");
		}
	}

	// free up pointers
	free(serverAddress);
	serverAddress = NULL;

	// add socket shutdown()
	close(clientSocket);
	return 0;
}




