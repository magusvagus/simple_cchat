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
	struct sockaddr_in *SERV_Address = SOCK_CreateAddressIPV4(ip, port);

	int ERR_Bind = bind(SERV_FileDisctiptor, (struct sockaddr *)SERV_Address, sizeof(*SERV_Address));
	if(ERR_Bind == -1) {
		printf("Error binding socket to port: %d.\n",port);
	}

	int ERR_Listen = listen(SERV_FileDisctiptor, backlog);
	if(ERR_Listen == -1) {
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

	int CLIEN_Quit = 1;
	while(CLIEN_Quit != 0) {
		// recv returns 0 when client quits connection
		CLIEN_Quit = 
			recv(CLIENT_FileDiscriptor, buffer, sizeof(buffer), 0);
		printf("%s\n", buffer);
	}

	free(SERV_Address);
	SERV_Address = NULL;

	return 0;
}

