#include "sockets.h"
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/select.h>


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
	//wrapper_main_loop(&serv_file_discriptor);
	fd_set read_fds;
	int sockfd = serv_file_discriptor;
	
	struct AcceptedSocket *sock_client_list = NULL;

	while (1) {
		// this function locks the program until
		// accept function confirms connection
		//
		// create a new client struct and point it 
		// to the user_list struct member
		struct AcceptedSocket *new_client = 
			sock_accept_client(serv_file_discriptor);
		new_client->user_list = sock_client_list;
		sock_client_list = new_client;

		pthread_t t1;
		pthread_create(&t1, NULL, wrapper_listen_print, sock_client_list );

		// send 10 msg's for testing
		for (int i = 0; i < 10; i++) {
			char hello[100];
			snprintf(hello, sizeof(hello), "Test message from server no: %d", i);
			send(sock_client_list->fileDiscriptor, hello, sizeof(hello), 0);
			sleep(2);
		}
	}

	// close all sockets
	//close(acceptedSocket->fileDiscriptor);
	shutdown(serv_file_discriptor, SHUT_RDWR);

	free(serv_address);
	serv_address = NULL;

	return 0;
}

