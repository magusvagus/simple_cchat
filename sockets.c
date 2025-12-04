#include "sockets.h"


// since function created on the stack wipe everything
// after the funciton returned, its importatnt to put
// the socket on the heap.
struct sockaddr_in* sock_create_IPV4_addr( char* ip, int port) 
{
	struct sockaddr_in *address;
	address = calloc(1, sizeof(struct sockaddr_in));

	address->sin_family = AF_INET;
	address->sin_port = htons(port);
	if( ip != NULL) {
		inet_pton(AF_INET, ip, &address->sin_addr.s_addr);
	}
	else {
		address->sin_addr.s_addr = INADDR_ANY;
	}

	return address;
}


struct AcceptedSocket* sock_accept_client(int serv_file_discriptor) 
{
	// accept returns the FD of the connecting client
	// and returns client's ip
	struct sockaddr_in client_address;
	int client_address_size = sizeof(client_address);

	int client_file_discriptor = 
		accept(serv_file_discriptor, 
				(struct sockaddr *)&client_address, &client_address_size);

	if(client_file_discriptor < 0) {
		printf("Error accepting client address.\n");
	}

	struct AcceptedSocket *s = 
		calloc(1, sizeof(struct AcceptedSocket));
	s->fileDiscriptor = client_file_discriptor;
	s->address = client_address;
	s->addressSize = client_address_size;
	
	return s;
}
