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
	int serv_file_discriptor = socket(AF_INET, SOCK_STREAM, 0);
	if(serv_file_discriptor == -1) {
		printf("Error creating file discriptor.\n");
	}

	//called memory must be freed
	struct sockaddr_in *serv_address = sock_create_IPV4_addr(NULL, port);

	int err_bind = bind(serv_file_discriptor, (struct sockaddr *)serv_address, sizeof(*serv_address));
	if(err_bind == -1) {
		printf("Error binding socket to port: %d.\n",port);
	}

	int err_listen = listen(serv_file_discriptor, backlog);
	if(err_listen == -1) {
		printf("Error listening on port: %d.\n" ,port);
	}

	// main loop thread
	wrapper_main_loop(&serv_file_discriptor);

	// blocking function for testing
	char test[100];
	fgets(test, sizeof(test), stdin);

	// close all sockets
	//close(acceptedSocket->fileDiscriptor);
	shutdown(serv_file_discriptor, SHUT_RDWR);

	free(serv_address);
	serv_address = NULL;

	return 0;
}

