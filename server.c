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
	int serv_file_disctiptor = socket(AF_INET, SOCK_STREAM, 0);
	if(serv_file_disctiptor == -1) {
		printf("Error creating file discriptor.\n");
	}

	//called memory must be freed
	struct sockaddr_in *serv_address = sock_create_IPV4_addr(ip, port);

	int err_bind = bind(serv_file_disctiptor, (struct sockaddr *)serv_address, sizeof(*serv_address));
	if(err_bind == -1) {
		printf("Error binding socket to port: %d.\n",port);
	}

	int err_listen = listen(serv_file_disctiptor, backlog);
	if(err_listen == -1) {
		printf("Error listening on port: %d.\n" ,port);
	}

	struct AcceptedSocket* acceptedSocket = sock_accept_client(serv_file_disctiptor);

	while(1) {
		int client_quit = recv(acceptedSocket->fileDiscriptor, 
				buffer, sizeof(buffer) - 1, 0);
		
		if (client_quit == 0) {
			printf("Client closed connection.\n");
			break;
		}
		printf("%s", buffer);
	}   

	// close all sockets
	close(acceptedSocket->fileDiscriptor);
	shutdown(serv_file_disctiptor, SHUT_RDWR);

	free(serv_address);
	serv_address = NULL;

	return 0;
}

