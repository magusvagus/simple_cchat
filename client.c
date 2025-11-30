#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // inet_pton


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

	int SOCK_FileDiscriptor =  socket(AF_INET,SOCK_STREAM,0);
	if (SOCK_FileDiscriptor == -1){
		printf("Error creating file discriptor.\n");
	}

	// test ip
	char* ip = "146.75.109.91";
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	// h to network short (big endian to liitle endinan)
	address.sin_port = htons(80); 
	// since the ip variable is a string, we need to convert each
	// of the numbers to a unsigned integer
	// (inet presentation to network)
	inet_pton(AF_INET, ip, &address.sin_addr.s_addr);


	// asks for a file discriptor, address , the length of address
	int err = connect(SOCK_FileDiscriptor, (struct sockaddr *)&address, sizeof(address));
	if(err == -1){
		printf("Error connecting to file discriptor\n");
	}else{
		printf("Connected\n");
	}




	return 0;
}
