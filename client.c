#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // inet_pton
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <ncurses.h>
#include <fcntl.h>
#include <errno.h>
#include "utils.h"
#include "sockets.h"


int main(void)
{
	/* --- sockets --- */

	int sock_fd = socket(AF_INET,SOCK_STREAM,0);
	if (sock_fd == -1){
		win_errpopup(NULL, NULL,"Error creating file discriptor.\n");
	}

	struct sockaddr_in *address = sock_create_IPV4_addr("127.0.0.1", 2000);

	int err = connect(sock_fd, (struct sockaddr *)address, sizeof(*address));
	if(err == -1){
		win_errpopup(NULL, NULL,"Error connecting to file discriptor\n");
	}else{
		printf("Connected\n");
	}


	// Set socket to non-blocking
	fd_set fd_bitmap;
	int flags = fcntl(sock_fd, F_GETFL, 0);
	fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK);



	/* --- chat ui --- */

	// ncurses
	initscr(); // start curses mode

	char nickname[17] = "";
	struct Win_ui ui = {0};
	ui.nickname = nickname;

	// initialize login window
	win_login_ui_init(&ui);
	refresh();
	wrefresh(ui.login_win->main);

	// write to login widow
	win_login_input(&ui, sock_fd);

	// remove \n 
	ui.nickname[strlen(ui.nickname) -1] = '\0';

	// initialise main ui (recv and send screen)
	win_main_ui_init(&ui);



	/* --- ncurses options --- */

	// disable cursor
	curs_set(0);

	// TODO: might not be needed for all
	nodelay(ui.send_win->sub, TRUE);
	nodelay(ui.recv_win->main, TRUE);
	nodelay(ui.recv_win->sub, TRUE);
	nodelay(stdscr, TRUE);

	win_reset(&ui);

	cbreak();
	timeout(0);
	noecho();
	keypad(stdscr, TRUE);



	/* --- main input loop --- */

	win_ui_input(&ui, sock_fd);



	/* --- deallocate memory --- */
	
	endwin(); // end curses mode
	
	// deallocate heap mem
	win_free(&ui);

	close(sock_fd);
	return 0;

}
