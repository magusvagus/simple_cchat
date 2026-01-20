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
// TODO: add more
// #define SIG_PRINT_LIST
// #define SIG_PRINT_STATUS
// #define SIG_KICK
// #define SIG_BAN
// #define SIG_TIME_ONLINE
// #define SIG_MSG_USER

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

// signal must have the same memory size across
// systems, since INTs and other data types have different
// size based on local architecture, a fixed sized data type 
// must be defined.
struct Signal {
	uint8_t type;
	char message[205];
	char nickname[20];
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
int sock_send_sig(int socket_fd, int SIG);

#endif


