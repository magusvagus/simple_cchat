#include "sockets.h"
#include <string.h>
#include <pthread.h>
#include <unistd.h>

int main(void)
{
	char ip[] = "0.0.0.0";
	char buffer[1024];
	int backlog = 50;
	int port = 2000;

	// create socket FD
	int SERV_FileDisctiptor = socket(AF_INET, SOCK_STREAM, 0);
	if(SERV_FileDisctiptor == -1) {
		printf("Error creating file discriptor.\n");
	}

	//called memory must be freed
	struct sockaddr_in *SERV_Address = SOCK_CreateAddressIPV4(ip, port);

	int ERR_Bind = bind(SERV_FileDisctiptor, (struct sockaddr *)SERV_Address, sizeof(*SERV_Address));
	if(ERR_Bind == -1) {
		printf("Error binding socket to port: %d.\n",port);
	}

	int ERR_Listen = listen(SERV_FileDisctiptor, backlog);
	if(ERR_Listen == -1) {
		printf("Error listening on port: %d.\n" ,port);
	}

	struct CLIENT_Socket* acceptedSocket = SOCK_AcceptClient(SERV_FileDisctiptor);

	while(1) {
		int CLIEN_Quit = recv(acceptedSocket->CLIENT_FileDiscriptor, 
				buffer, sizeof(buffer) - 1, 0);
		
		if (CLIEN_Quit == 0) {
			printf("Client closed connection.\n");
			break;
		}
		printf("%s", buffer);
	}   

	// close all sockets
	close(acceptedSocket->CLIENT_FileDiscriptor);
	shutdown(SERV_FileDisctiptor, SHUT_RDWR);

	free(SERV_Address);
	SERV_Address = NULL;

	return 0;
}

