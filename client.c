#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // inet_pton
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <ncurses.h>
#include <ctype.h>


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

	/* 
	The socket file descriptor (FD) is not fixed — it gets the next available integer in 
	the process's FD table when created (e.g., 3, 4, etc.).
	So while 0, 1, 2 are reserved for stdin, stdout, stderr, a socket can be assigned 
	any higher number — or even reuse 0, 1, 2 if they were closed. 
	*/
	fd_set fd_bitmap;
	int sock_fd = SOCK_FileDiscriptor;

	// ncurses
	initscr(); // start curses mode
	
	// rs - root screan
	int rs_row;
	int rs_col;

	getmaxyx(stdscr,rs_row,rs_col);

	WINDOW *recv_win;
	WINDOW *send_win;

	recv_win = newwin( rs_row - 4, rs_col, NULL, NULL);
	send_win = newwin(4, rs_col, rs_row - 4, NULL);

	box(recv_win, 0,0);
	box(send_win, 0,0);
	
	nodelay(send_win, TRUE);
	nodelay(recv_win, TRUE);

	refresh();
	wrefresh(send_win);
	wrefresh(recv_win);
	
	// window title
	mvwprintw(recv_win, 0, 1, "Chatroom");
	wrefresh(recv_win);

	int ch;
	int pos = 0;

	cbreak();
	nodelay(stdscr, TRUE);
	timeout(0);
	noecho();
	keypad(stdscr, TRUE);

	int i = 0;
	int c = 1;

while (1) {
    // Redraw prompt and current input
    mvwprintw(send_win, 1, 1, "%s: %s", nickname, message);
    wrefresh(send_win);

	ch = getch();

	if (ch != ERR) {
		// TODO needs check for backspace
		if (ch == '\n' || ch == '\r') {
			message[i] = '\n';
			int ERR_send = send(SOCK_FileDiscriptor, message, strlen(message), 0);
			if(ERR_send == -1) {
				printf("Error, could not send message.\n");
			}
			i = 0;
			memset(message, 0, sizeof(message));
			wmove(send_win, 1,1);
			wclrtoeol(send_win); // clear line to end
    		mvwprintw(send_win, 1, 1, "%s: %s", nickname, message);
			wrefresh(send_win);
		}
		else if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
			i--;
			message[i] = ' ';
		}
		else {
			message[i] = ch;
			wmove(send_win, 1, 1);
			wrefresh(send_win);
			i++;
		}
	}

		//fgets(message, sizeof(message), stdin);

		// TODO fix, and adjust for ncurses
		// if(!strcmp(message, "/quit\n")) {
		// 	close(SOCK_FileDiscriptor);
		// 	break;
		// }

		// int ERR_send = send(SOCK_FileDiscriptor, message, sizeof(message), 0);
		// if(ERR_send == -1) {
		// 	printf("Error, could not send message.\n");
		// }
		// message[0] = '\0';


		// TODO recv blocks loop, might use select() or change socket flags to not block loop
		// message recieved
		//char r_msg[256];
		char r_msg[] = "test message";
		//int client_quit = recv(SOCK_FileDiscriptor, r_msg, sizeof(r_msg), 0);
		mvwprintw(recv_win, 1, 1, "Recieved: %s %d\n",r_msg, c);
		c++;
		wrefresh(recv_win);

		//printf("Recieved: %s\n",r_msg);
		r_msg[0] = '\0';

		usleep(10000);
	}

	endwin(); // end curses mode
	close(SOCK_FileDiscriptor);
	return 0;
}
