#ifndef SOCKETS_H
#define SOCKETS_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <stdio.h>
#include <stdlib.h>


struct sockaddr_in* SOCK_CreateAddressIPV4( char* ip, int port);

#endif


