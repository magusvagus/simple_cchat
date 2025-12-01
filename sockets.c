#include "sockets.h"

int SOCK_CreateTCP_IPV4( char* ip, int port) 
{
	int SOCK_FileDisctiptor = socket(AF_INET, SOCK_STREAM, 0);
	if(SOCK_FileDisctiptor == -1) {
		printf("Error creating file discriptor.\n");
	}
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	inet_pton(AF_INET, ip, &address.sin_addr.s_addr);

	return SOCK_FileDisctiptor;
}
