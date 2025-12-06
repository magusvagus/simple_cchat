#include "sockets.h"
#include <unistd.h>
#include <string.h>
#include <time.h>

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
	if(client_file_discriptor == -1) {
		printf("Error accepting client address.\n");
	}

	// save time the user joined
	time_t login_timestamp;
	time( &login_timestamp );
	struct tm *ts = localtime(&login_timestamp);

	struct AcceptedSocket *s = 
		calloc(1, sizeof(struct AcceptedSocket));
	s->fileDiscriptor = client_file_discriptor;
	s->address = client_address;
	s->addressSize = client_address_size;
	s->timestamp_raw = login_timestamp;

	snprintf(s->timestamp_formatted, sizeof(s->timestamp_formatted), 
			"%d:%d:%d", ts->tm_hour,ts->tm_min,ts->tm_sec);
	
	return s;
}

void sock_listen_print(struct AcceptedSocket *acceptedSocket)
{
	char buffer[1024];
	// TODO add nickname and user ID to struct
	// struct {id, nickname, time when joined}
	char nickname[17];

	// ask for nickname
	int client_quit = recv(acceptedSocket->fileDiscriptor, 
			nickname, sizeof(nickname), 0);

	// remove \n 
	nickname[strlen(nickname) -1] = '\0';

	// time of entering chat
	time_t login_timestamp;
	time(&login_timestamp);
	struct tm *ts = localtime(&login_timestamp);

	printf("%d:%d:%d %s entered chat\n",
			ts->tm_hour,ts->tm_min,ts->tm_sec, nickname);

	// print recieve loop
	while(1) {
		int client_quit = recv(acceptedSocket->fileDiscriptor, 
				buffer, sizeof(buffer), 0);
		
		if (client_quit == 0) {
			printf("%s closed the connection.\n", nickname);
			break;
		}

		// print message w/ time
		time(&login_timestamp);
		struct tm *ts = localtime(&login_timestamp);
		printf("%d:%d:%d %s: %s",ts->tm_hour,ts->tm_min,ts->tm_sec, nickname, buffer);

		// reset buffer
		buffer[0] = '\0';
	}   
	close(acceptedSocket->fileDiscriptor);
}

void* wrapper_listen_print(void* arg)
{
	struct AcceptedSocket* AcceptedSocket = (struct AcceptedSocket*)arg;
	sock_listen_print(AcceptedSocket);
	return NULL;
}
