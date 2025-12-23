#include "sockets.h"
#include "utils.h"
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

struct Serv_options serv_option = {0};
struct tm *ts = NULL;

// since function created on the stack wipe everything
// after the funciton returned, its importatnt to put
// the socket on the heap.
struct sockaddr_in* 
sock_create_IPV4_addr( char* ip, int port) 
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


struct AcceptedSocket* 
sock_accept_client(int serv_file_discriptor) 
{
	static int client_id = 0;

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

	// set timestamp
	ts = timestamp();

	struct AcceptedSocket *s = 
		calloc(1, sizeof(struct AcceptedSocket));
	s->fileDiscriptor = client_file_discriptor;
	s->address = client_address;
	s->addressSize = client_address_size;
	// TODO check if raw time is in ts struct
	//s->timestamp_raw = login_timestamp;
	s->client_id = client_id++;

	snprintf(s->timestamp_formatted, sizeof(s->timestamp_formatted), 
			"%02d:%02d:%02d", ts->tm_hour,ts->tm_min,ts->tm_sec);
	
	return s;
}


void 
sock_listen_print(struct AcceptedSocket *acceptedSocket)
{
	char buffer[1024];
	char nickname[17];

	// ask for nickname
	int client_quit = recv(acceptedSocket->fileDiscriptor, 
			nickname, sizeof(nickname), 0);

	// remove \n 
	nickname[strlen(nickname) -1] = '\0';

	// fill struct with nickname
	strcpy(acceptedSocket->nickname, nickname);
 
	// time of entering chat
	time_t login_timestamp;
	time(&login_timestamp);
	struct tm *ts = localtime(&login_timestamp);

	printf("%02d:%02d:%02d %s entered chat\n",
			ts->tm_hour,ts->tm_min,ts->tm_sec, nickname);

	// print recieve loop
	while(1) {
		int client_quit = 
			recv(acceptedSocket->fileDiscriptor, buffer, sizeof(buffer), 0);

		if ( client_quit > 0) {
			buffer[client_quit] = '\0';
		}

		// update timestamp
		ts = timestamp();

		if (client_quit == 0) {
			printf("%02d:%02d:%02d %s closed the connection.\n",
					ts->tm_hour,ts->tm_min,ts->tm_sec, nickname);
			break;
		}

		// update timestamp
		ts = timestamp();

		// TODO send back/ echo the message to the client/s
		printf("%02d:%02d:%02d %s: %s",
				ts->tm_hour,ts->tm_min,ts->tm_sec, nickname, buffer);

		send(acceptedSocket->fileDiscriptor, buffer, sizeof(buffer), 0);

		// reset buffer
		buffer[0] = '\0';
	}   
	close(acceptedSocket->fileDiscriptor);
}


void* 
wrapper_listen_print(void* arg)
{
	struct AcceptedSocket* AcceptedSocket = (struct AcceptedSocket*)arg;
	sock_listen_print(AcceptedSocket);
	return NULL;
}

