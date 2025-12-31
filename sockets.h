#ifndef SOCKETS_H
#define SOCKETS_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <stdio.h>
#include <stdlib.h>

#define SIG_MSG   1
#define SIG_QUIT  2
#define SIG_NICK  3

struct AcceptedSocket {
	int fileDiscriptor;
	int addressSize;
	struct sockaddr_in address;

	char nickname[20];

	int kicked_status;
	int client_id;
	int timestamp_raw;
	char timestamp_formatted[20];

	// user_list points to previous created list
	// *user_list <- *user <-*user2 etc.
	struct AcceptedSocket *user_list;
};

struct Signal {
	int type;
	char message;
	char nickname;
};

struct Serv_options {
	int quit_forcefully;
	int quit_gracefully;
	int print_user_list;
	int print_user_timestamp;
	int kick_user;
	int message_user;
};

struct Serv_main_loop {
	struct AcceptedSocket *s;
	int serv_file_discriptor;
	int *quit;
};

extern struct Serv_options serv_option;

struct sockaddr_in* sock_create_IPV4_addr( char* ip, int port);
struct AcceptedSocket* sock_accept_client(int SERV_FileDisctiptor);
void sock_listen_print(struct AcceptedSocket *acceptedSocket);
void* wrapper_listen_print(void* arg);

#endif


