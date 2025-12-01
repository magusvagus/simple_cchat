#include "sockets.h"

int main(void)
{
	char *ip = "172.0.0.1";

	// create socket FD
	int SOCK_FileDisctiptor = socket(AF_INET, SOCK_STREAM, 0);
	if(SOCK_FileDisctiptor == -1) {
		printf("Error creating file discriptor.\n");
	}

	//called memory must be freed
	struct sockaddr_in *address = SOCK_CreateAdressIPV4(ip, 80);

	free(address);
	address = NULL;
	return 0;
}

