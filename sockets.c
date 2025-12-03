#include "sockets.h"


// since function created on the stack wipe everything
// after the funciton returned, its importatnt to put
// the socket on the heap.
struct sockaddr_in* SOCK_CreateAddressIPV4( char* ip, int port) 
{
	struct sockaddr_in *address;
	address = calloc(1, sizeof(struct sockaddr_in));

	address->sin_family = AF_INET;
	address->sin_port = htons(port);
	inet_pton(AF_INET, ip, &address->sin_addr.s_addr);

	return address;
}


struct CLIENT_Socket* SOCK_AcceptClient(int SERV_FileDisctiptor) 
{
	// accept returns the FD of the connecting client
	// and returns client's ip
	struct sockaddr_in CLIENT_Address;
	int CLIENT_AddressSize = sizeof(CLIENT_Address);

	int CLIENT_FileDiscriptor = 
		accept(SERV_FileDisctiptor, 
				(struct sockaddr *)&CLIENT_Address, &CLIENT_AddressSize);

	if(CLIENT_FileDiscriptor < 0) {
		printf("Error accepting client address.\n");
	}

	struct CLIENT_Socket* acceptedSocket = calloc(1, sizeof(struct CLIENT_Socket));
	acceptedSocket->CLIENT_FileDiscriptor = CLIENT_FileDiscriptor;
	acceptedSocket->address = CLIENT_Address;
	acceptedSocket->CLIENT_AddressSize = CLIENT_AddressSize;
	
	return acceptedSocket;
}
