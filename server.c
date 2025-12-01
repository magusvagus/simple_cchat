#include <stdio.h>
#include "sockets.h"

int main(void)
{
	char *ip = "172.0.0.1";
	SOCK_CreateTCP_IPV4(ip, 80);
	

	return 0;
}

