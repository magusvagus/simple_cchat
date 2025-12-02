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
