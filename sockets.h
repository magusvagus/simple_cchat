#ifndef SOCKETS_H
#define SOCKETS_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <stdio.h>
#include <stdlib.h>

struct CLIENT_Socket {
	int CLIENT_FileDiscriptor;
	int CLIENT_AddressSize;
	struct sockaddr_in address;
};

struct sockaddr_in* SOCK_CreateAddressIPV4( char* ip, int port);
struct CLIENT_Socket* SOCK_AcceptClient(int SERV_FileDisctiptor);

#endif


