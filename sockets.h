#ifndef SOCKETS_H
#define SOCKETS_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <stdio.h>
#include <stdlib.h>

struct AcceptedSocket {
	int fileDiscriptor;
	int addressSize;
	struct sockaddr_in address;

	char nickname[20];

	int client_id;
	int timestamp_raw;
	char timestamp_formatted[20];
};

struct sockaddr_in* sock_create_IPV4_addr( char* ip, int port);
struct AcceptedSocket* sock_accept_client(int SERV_FileDisctiptor);
void sock_listen_print(struct AcceptedSocket *acceptedSocket);
void* wrapper_listen_print(void* arg);

#endif


