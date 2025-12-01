#ifndef SOCKETS_H
#define SOCKETS_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <stdio.h>


int SOCK_CreateTCP_IPV4( char* ip, int port);

#endif


