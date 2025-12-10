#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // inet_pton
#include <string.h>
#include <unistd.h>
#include <sys/select.h>


int main(void)
{
	/*
	Socket function requires 3 parameters
	- adress family (ipv4/ ipv6) 
		AF_INET = adress family ipv4
	- socket type (tcp/ udp)
	- protocol layer to be used underneth socket layer
		TCP/UDP are OSI layer 4, this value
		defines the layer underneth to be given
		to after the socket is done processing

		O defines using IP, but it can be configured
		to use different network layers
	- returns INT -1 if error occurs
	*/ 		

	int SOCK_FileDiscriptor = socket(AF_INET,SOCK_STREAM,0);
	if (SOCK_FileDiscriptor == -1){
		printf("Error creating file discriptor.\n");
	}

	// test ip
	char* ip = "127.0.0.1";
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	// h to network short (big endian to liitle endinan)
	address.sin_port = htons(2000); 
	// since the ip variable is a string, we need to convert each
	// of the numbers to a unsigned integer
	// (inet presentation to network)
	inet_pton(AF_INET, ip, &address.sin_addr.s_addr);


	// asks for a file discriptor, address , the length of address
	// since sockaddr is a generic struct for several addr types
	// and lacks struct members like sin_port and sin_family.
	// this means we need to use the ipv4 struct (sockaddr_in) and cast it to
	// sockaddr, it tells the compiler to treat the memmory address of sockaddr_in
	// struct as if it points to sockaddr.
	int err = connect(SOCK_FileDiscriptor, (struct sockaddr *)&address, sizeof(address));
	if(err == -1){
		printf("Error connecting to file discriptor\n");
	}else{
		printf("Connected\n");
	}
	// the cast gives the function access to the (sa_family) member of the struct
	// wich defines its family type (AF_INET) in this case.
	// the function accepts a generic (sockaddr *) but, interprets it based on the
	// (sa_family) struct member allowing it to work with different address types.

	// The (sa_family) member identifies the 'address family' for local sockets.
	// it tells the system how to interpret the rest of the address structure.
	// When a generic (struct sockaddr *) is passed to the function, the system
	// reads (sa_family) to determine the correct specific structure type (lise sockaddr_in)

	char message[1024];
	char buffer[1024];
	char nickname[17];

	// send nickname to server
	while(1) {
		printf("Enter nickname: ");
		fgets(nickname, sizeof(nickname), stdin);
		if(strlen(nickname) <= 1) {
			printf("Nickname too short (2 - 15 characters.)");
		}
		else if (strlen(nickname) > 15) {
			printf("Nickname too long (2 - 15 characters.)");
		} 
		else {
			break;
		}
	}

	int ERR_send = send(SOCK_FileDiscriptor, nickname, sizeof(nickname), 0);
	if(ERR_send == -1) {
		printf("Error, could not send nickname.\n");
	}

	// remove \n 
	nickname[strlen(nickname) -1] = '\0';

	fd_set read_fds;
	int sockfd = SOCK_FileDiscriptor;

	while(1) {
		FD_ZERO(&read_fds);
		FD_SET(0, &read_fds);
		FD_SET(sockfd, &read_fds);

		if (select(sockfd + 1, &read_fds, NULL, NULL, NULL) > 0) {
			if (FD_ISSET(0, &read_fds)) {

				printf("%s: ",nickname);
				char message[256];
				fgets(message, sizeof(message), stdin);

				if(!strcmp(message, "/quit\n")) {
					close(SOCK_FileDiscriptor);
					break;
				}

				int ERR_send = send(SOCK_FileDiscriptor, message, sizeof(message), 0);
				if(ERR_send == -1) {
					printf("Error, could not send message.\n");
				}
				message[0] = '\0';
			}
		}
		if (FD_ISSET(sockfd, &read_fds)) {
			// message recieved
			char r_msg[256];
			int client_quit = recv(SOCK_FileDiscriptor, 
					r_msg, sizeof(r_msg), 0);

			printf("Recieved: %s\n",r_msg);
			r_msg[0] = '\0';
		}
	}
	close(SOCK_FileDiscriptor);
	return 0;
}
