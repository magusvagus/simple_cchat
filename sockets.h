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
};

struct sockaddr_in* sock_create_IPV4_addr( char* ip, int port);
struct AcceptedSocket* sock_accept_client(int SERV_FileDisctiptor);

#endif


