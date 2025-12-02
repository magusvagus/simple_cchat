#include "sockets.h"

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
	struct sockaddr_in *address = SOCK_CreateAddressIPV4(ip, port);

	int err_bind = bind(SERV_FileDisctiptor, (struct sockaddr *)address, sizeof(*address));
	if(err_bind == -1) {
		printf("Error binding socket to port: %d.\n",port);
	}

	int err_listen = listen(SERV_FileDisctiptor, backlog);
	if(err_listen == -1) {
		printf("Error listening on port: %d.\n" ,port);
	}

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

	recv(CLIENT_FileDiscriptor, buffer, sizeof(buffer), 0);
	printf("%s", buffer);


	free(address);
	address = NULL;

	return 0;
}

