#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#include <string.h> // for strerrno()
#include <errno.h> // for error handling
#include <pthread.h> // multithreading

#include <arpa/inet.h> // for inet_addr function
#include <sys/types.h> // for definition of socket functions
#include <sys/socket.h> // <- most important for sockets

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <semaphore.h>

#include "functions.h"

#define QUEUE 10

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

int main(void)
{
	// define socket, familly, socket type, protocol
	int serverSocket = 
		create_TCP_socket();

	// define client address
	struct sockaddr_in *clientAddress = 
		create_IPV4_address(9002, "127.0.0.1");

	// check malloc from function above
	if (clientAddress == NULL) 
	{
		fprintf(stderr, "[ ER ] ERROR, memory allocation failed\n");
		printf("\tErrno: %d\n",errno);
		printf("\tErrmsg: %s\n",strerror(errno));
		return -1;
	}

	// bind to incoming socket connection
	int bindStatus;
	bindStatus = bind(serverSocket, 
				(struct sockaddr *) clientAddress, 
				sizeof(*clientAddress));

	if (bindStatus == -1) 
	{
		printf("[ ER ] ERROR, binding failed.\n");
		printf("\tErrno: %d\n",errno);
		printf("\tErrmsg: %s\n",strerror(errno));

	} else {
		printf("[ OK ] Server connection established.\n");
	}

	// backlog amount
	int listenResult;
	listenResult = listen(serverSocket, QUEUE);

	if (listenResult == -1) 
	{
		printf("[ ER ] Error, listening failed\n");
		printf("\tErrno: %d\n",errno);
		printf("\tErrmsg: %s\n",strerror(errno));
	}
	else
	{
		printf("[ OK ] Listen successful.\n");
	}

	while (1) 
	{
		// accept socket
		struct AcceptSocket *clientSocket = 
			acceptSocket(serverSocket);

		pthread_t thread; // define threat
		int *newsock = malloc(sizeof(int)); // memory for now socket

		if (newsock == NULL) 
		{
			printf("[ ER ] Errror, socket memory allocation failed!\n");
			return 1;
		}

		*newsock = clientSocket->acceptedSocket;
		pthread_create(&thread, NULL, run_thread, newsock);
		// this changes everything, i need to ivestigate
		pthread_detach(thread);

		// server options menu
		// char serverOptions[20];
		// fgets(serverOptions, sizeof(serverOptions), stdin);
		// serverOptions[strlen(serverOptions) - 1] = '\0';
	}

	// close sockets
	// add socket shutdown()
	//close(clientSocket);
	//free(newsock);

	free(clientAddress);
	free(acceptSocket);

	close(serverSocket);

	clientAddress = NULL;
	return 0;
}


