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
	int SOCK_FileDiscriptor = socket(AF_INET,SOCK_STREAM,0);
	if (SOCK_FileDiscriptor == -1){
		win_errpopup(NULL, NULL,"Error creating file discriptor.\n");
	}

	struct sockaddr_in *address = sock_create_IPV4_addr("127.0.0.1", 2000);

	int err = connect(SOCK_FileDiscriptor, (struct sockaddr *)address, sizeof(*address));
	if(err == -1){
		win_errpopup(NULL, NULL,"Error connecting to file discriptor\n");
	}else{
		printf("Connected\n");
	}

	char nickname[17];

	// ncurses
	initscr(); // start curses mode

	struct Win_ui ui = {0};
	ui.nickname = nickname;
	win_login_ui_init(&ui);

	refresh();
	wrefresh(ui.login_win->main);

	win_login_input(&ui, SOCK_FileDiscriptor);

	int ch;
	int i = 0;

	// remove \n 
	ui.nickname[strlen(ui.nickname) -1] = '\0';

	fd_set fd_bitmap;
	int sock_fd = SOCK_FileDiscriptor;

	win_main_ui_init(&ui);

	// disable cursor
	curs_set(0);

	// TODO: might not be needed for all
	nodelay(ui.send_win->sub, TRUE);
	nodelay(ui.recv_win->main, TRUE);
	nodelay(ui.recv_win->sub, TRUE);

	win_reset(&ui);

	// disable cursor
	curs_set(0);

	cbreak();
	nodelay(stdscr, TRUE);
	timeout(0);
	noecho();
	keypad(stdscr, TRUE);

	i = 0;
	int test = 0;

	// Set socket to non-blocking
	int flags = fcntl(SOCK_FileDiscriptor, F_GETFL, 0);
	fcntl(SOCK_FileDiscriptor, F_SETFL, flags | O_NONBLOCK);

	win_ui_input(&ui, SOCK_FileDiscriptor);

	// TODO: move to seperate function
	// while (1) {
	// 	// refresh boxes
	// 	touchwin(stdscr);
	// 	touchwin(ui.send_win->main);
	// 	touchwin(ui.recv_win->main);
	//
	// 	// Redraw prompt and current input
	// 	mvwprintw(ui.send_win->sub, 0, 0, "%s: %s", nickname, message);
	// 	win_reset(&ui);
	//
	// 	// test error function
	// 	if (!test) {
	// 		win_errpopup(NULL, "TEST ERR","test error");
	// 		test = 1;
	// 	}
	//
	// 	ch = wgetch(ui.send_win->sub);
	//
	// 	if (ch != ERR) {
	// 		if (ch == '\n' || ch == '\r') {
	// 			message[i] = '\n';
	//
	// 			// TODO put into function/ struct with other commands
	// 			if (!strcmp(message, "/quit\n")) {
	// 				close(SOCK_FileDiscriptor);
	// 				break;
	// 			}
	// 			// if message is just enter, do nothing
	// 			else if ( message[0] == '\n') {
	// 				continue;
	// 			}
	//
	// 			// TODO instead of sending raw text buffer, an type struct should be only send/ recieved
	// 			// so one ui.recv/send function can parse multiple types of signals/ requests
	// 			int ERR_send = send(SOCK_FileDiscriptor, message, strlen(message), 0);
	// 			if(ERR_send == -1) {
	// 				win_errpopup(NULL, NULL,"Error, could not send message.\n");
	// 			}
	//
	// 			i = 0;
	// 			memset(message, 0, sizeof(message));
	// 			wmove(ui.send_win->sub, 0,1);
	// 			wclrtoeol(ui.send_win->sub); // clear line to end
	// 			mvwprintw(ui.send_win->sub, 0, 0, "%s: %s", nickname, message);
	// 			win_reset(&ui);
	// 		}
	// 		else if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
	// 			// reprint, removing character and set to null
	// 			// otherwise user can send deleted string as sequence of empty spaces
	// 			if (i > 0) {
	// 				i--;
	// 				message[i] = ' ';
	// 				mvwprintw(ui.send_win->sub, 0, 0, "%s: %s", nickname, message);
	// 				message[i] = '\0';
	// 			}
	// 			else {
	// 				i=0;
	// 			}
	// 		}
	// 		else {
	// 			message[i] = ch;
	// 			wmove(ui.send_win->sub, 0, 0);
	// 			wrefresh(ui.send_win->sub);
	// 			wrefresh(ui.send_win->main);
	// 			i++;
	// 		}
	// 	}
	//
	// 	char r_msg[256];
	// 	int client_quit = recv(SOCK_FileDiscriptor, r_msg, sizeof(r_msg), 0);
	// 	if (client_quit > 0 && r_msg[0] != '\0') {
	// 		// TODO: add timestamp and nick of sender
	// 		wprintw(ui.recv_win->sub, "Recieved: %s",r_msg);
	// 		touchwin(ui.recv_win->main);
	// 		win_reset(&ui);
	// 		r_msg[0] = '\0';
	// 	} 
	// 	else if (client_quit == 0) {
	// 		win_errpopup(NULL,NULL,"Server closed connection");
	// 		break;
	//
	// 	}
	// 	else {
	// 		if (errno == EAGAIN || errno == EWOULDBLOCK) {
	// 			// No data now — continue looping
	// 			continue;
	// 		} 
	// 		else {
	// 			// Real error
	// 			win_errpopup(NULL,NULL,"Error sending message to server");
	// 			break;
	// 		}
	// 	}
	// 	usleep(100000);
	// }
	
	endwin(); // end curses mode
	
	// deallocate heap mem
	win_free(&ui);

	close(SOCK_FileDiscriptor);
	return 0;
}
