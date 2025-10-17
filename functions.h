#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include <string.h> // for strerrno()
#include <errno.h> // for error handling
#include <pthread.h> // multithreading

#include <arpa/inet.h> // for inet_addr function
#include <sys/types.h> // for definition of socket functions
#include <sys/socket.h> // <- most important for sockets

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>


// struct for taking user data from accepted socket
struct AcceptSocket{
	// accept socket (returns client FD/socket)
	int acceptedSocket;
	struct sockaddr_in address;
	int addressSize;
	int error;
	char nickname[100];
	bool acceptedSuccess;
};

// function prototypes
struct sockaddr_in *create_IPV4_address(int port, char *ip);
int create_TCP_socket();
struct AcceptSocket *acceptSocket(int serverSocket);
void *threadedSocket(void *serverSocket);
void *run_thread(void *serverSocket);


void *run_thread(void *intClientSocket)
{
	// set timer for message timer
	time_t rawtime = 0; // raw time integer
	struct tm *pTime = NULL; // time.h struct
	time(&rawtime); // update with current time
	pTime = localtime(&rawtime); // fill time.h struct with local time

	// dereferece external *void socket to int datat type
	int clientSocket = *(int*)intClientSocket;

	// server msg to the client
	char welcomeMsg[1024] = "Welcome to the fucked up server\n";
	send(clientSocket, welcomeMsg, sizeof(welcomeMsg), 0);
	
	// ask for nickname
	char nickname[100];
	char nickMsg[1024] = "Enter your nickname: \n";
	send(clientSocket, nickMsg, sizeof(nickMsg), 0);

	char clientResponse[1024] = "\0";
	recv(clientSocket, &clientResponse, sizeof(clientResponse), 0);
	clientResponse[strlen(clientResponse)-1] = '\0';
	strcpy(nickname, clientResponse);

	printf(" %02d:%02d | %s enters the chat.\n",
			pTime->tm_hour, 
			pTime->tm_min, 
			nickname
			);

	// message loop start
	int loop = 1;
	while (loop) 
	{
		time(&rawtime); // update with current time
		pTime = localtime(&rawtime); // fill time.h struct with local time

		char clientResponse[1024] = "\0";
		int closeCheck;
		closeCheck = 
			recv(clientSocket, 
				&clientResponse, 
				sizeof(clientResponse), 0);

		// check if socket is running (not offline)
		if (closeCheck == 0 ) 
		{
			printf(" %02d:%02d | Connection closed by %s.\n", 
					pTime->tm_hour,
					pTime->tm_min,
					nickname
					);

			loop = 0;
			break;
		}

		printf(" %02d:%02d | %s: %s",
				pTime->tm_hour,
				pTime->tm_min,
				nickname, 
				clientResponse
				);
	}
	intClientSocket = NULL;
	pthread_exit(NULL);
	return NULL;
}


struct AcceptSocket *acceptSocket(int serverSocket){
	// accept socket (returns client FD/socket)
	struct sockaddr_in clientAddress;
	int clientAddressSize = sizeof(struct sockaddr_in);

	int clientSocket = accept(serverSocket, 
				(struct sockaddr *) &clientAddress, 
				&clientAddressSize);

	// create client socket struct
	struct AcceptSocket *acceptSocket = 
		malloc( sizeof(struct AcceptSocket) );
	acceptSocket->address = clientAddress;
	acceptSocket->acceptedSocket = clientSocket;
	acceptSocket->acceptedSuccess = clientSocket>0;

	// if socket success == 0
	if(!acceptSocket->acceptedSuccess){
		acceptSocket->error = clientSocket;
		printf("[ ER ] Error, client not accepted\n");
		printf("\tErrno: %d\n",errno);
		printf("\tErrmsg: %s\n",strerror(errno));
	}
	else{
		printf("[ OK ] Client accepted, connection established.\n");
	}

	return acceptSocket;
}


struct sockaddr_in *create_IPV4_address(int port, char *ip){
	// define address
	// after the function is initialised the data from here
	// can be overwritten, so we have to define it using malloc
	// so it stays in memory.
	
	struct sockaddr_in *serverAddress = 
		malloc(sizeof(struct sockaddr_in));

	serverAddress->sin_family = AF_INET;
	serverAddress->sin_port = htons(port);
	serverAddress->sin_addr.s_addr = inet_addr(ip);
	return serverAddress;
}


int create_TCP_socket(){
	// define socket, familly, socket type, undrlying protocol
	int clientSocket;
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket == -1){
		printf("\n[ ER ] ERROR, failed socket creation\n");
		printf("\tErrno: %d\n",errno);
		printf("\tErrmsg: %s\n",strerror(errno));
	}
	else{
		printf("\n[ OK ] Socket initialzation successful.\n");
	}
	return clientSocket;
}

#endif // MYHEADER_H   
