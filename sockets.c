#include "sockets.h"
#include "utils.h"
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

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

	struct AcceptedSocket *s = calloc(1, sizeof(struct AcceptedSocket));

	s->fileDiscriptor = client_file_discriptor;
	s->address = client_address;
	s->addressSize = client_address_size;
	s->client_id = client_id++;

	snprintf(s->timestamp_formatted, sizeof(s->timestamp_formatted), 
			"%02d:%02d:%02d", ts->tm_hour,ts->tm_min,ts->tm_sec);
	
	return s;
}


void 
sock_listen_print(struct AcceptedSocket *acceptedSocket)
{
	char buffer[210];
	char nickname[17];
	struct Packet pak = {0};

	// ask for nickname
	int client_quit = 
		recv(acceptedSocket->fileDiscriptor, nickname, sizeof(nickname), 0);

	// remove \n 
	nickname[strlen(nickname) -1] = '\0';

	// fill user struct with nickname
	strcpy(acceptedSocket->nickname, nickname);
 
	// time of entering chat
	time_t login_timestamp;
	time(&login_timestamp);
	struct tm *ts = localtime(&login_timestamp);

	printf("%02d:%02d:%02d %s entered chat.\n",
			ts->tm_hour,ts->tm_min,ts->tm_sec, nickname);

	// send/ recieve loop
	while(1) {
		// recieve message from client
		int client_quit = 
			recv(acceptedSocket->fileDiscriptor, buffer, sizeof(buffer), 0);
			sock_read_packet(buffer, &pak);

		if ( client_quit > 0) {
			buffer[client_quit] = '\0';
		}

		// check for client to quit
		if (client_quit == 0) {
			// get timestamp
			ts = timestamp();

			printf("%02d:%02d:%02d %s -> closed the connection.\n",
					ts->tm_hour,ts->tm_min,ts->tm_sec, nickname);
			break;
		}

		// update timestamp
		ts = timestamp();

		// show message
		printf("%02d:%02d:%02d %s: %s",
				ts->tm_hour,ts->tm_min,ts->tm_sec, nickname, pak.message);

		// TODO: re-serialize packet?
		// send/ echo back to client
		//pak.type_test = SIG_MSG;
		//strcpy(pak.message, message);
		//sock_serialize_packet(&pak);
		//sock_send_sig(acceptedSocket->fileDiscriptor, &pak);
		send(acceptedSocket->fileDiscriptor, pak.message, sizeof(pak.message), 0);

		// reset buffer
		buffer[0] = '\0';
		pak.message[0] = '\0';
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

int
sock_send_sig(int socket_fd, struct Packet *pak)
{
	int err_send = send(socket_fd, pak->buffer, sizeof(pak->buffer), 0);
	if(err_send == -1) {
		win_errpopup(NULL, NULL,"Error, could not send nickname.\n");
		return err_send;
	}
	return 0;
}

// TODO: add nickname to buffer
void
sock_serialize_packet(struct Packet *pak)
{
	char *ptr = pak->buffer;
	pak->buffer[0] = (pak->type_test >> 24) & 0xFF;
	pak->buffer[1] = (pak->type_test >> 16) & 0xFF;
	pak->buffer[2] = (pak->type_test >> 8) & 0xFF;
	pak->buffer[3] = pak->type_test & 0xFF;

	for (int i = 0; i < 205; i++) {
		pak->buffer[4 + i] = pak->message[i];
	}
}

// TODO: add reading of nickname inside raw buffer
void
sock_read_packet(char *raw_buffer, struct Packet *pak)
{
	// read type_test in big-endian
	uint32_t type_test = 
		(uint32_t)raw_buffer[0] << 24 |
		(uint32_t)raw_buffer[1] << 16 |
		(uint32_t)raw_buffer[2] << 8  |
		(uint32_t)raw_buffer[3];

	pak->type_test = type_test;

	for (int i = 0; i < 205; i++) {
		pak->message[i] = raw_buffer[4 + i];
	}
}
