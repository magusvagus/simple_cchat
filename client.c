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
		err_screen(NULL, NULL,"Error creating file discriptor.\n");
	}

	struct sockaddr_in *address = sock_create_IPV4_addr("127.0.0.1", 2000);

	int err = connect(SOCK_FileDiscriptor, (struct sockaddr *)address, sizeof(*address));
	if(err == -1){
		err_screen(NULL, NULL,"Error connecting to file discriptor\n");
	}else{
		printf("Connected\n");
	}

	char message[1024];
	char buffer[1024];
	char nickname[17];

	// ncurses
	initscr(); // start curses mode
	// rs - root screan
	int rs_row;
	int rs_col;
	int log_winy = 10;
	int log_winx = 40;
	getmaxyx(stdscr,rs_row,rs_col);

	struct Win_nested *log_win = NULL;
	log_win = win_nested(0, log_winy, log_winx,0,0, 1);
	if (log_win == NULL) {
		err_screen(NULL, NULL,"Error creating login window\n");
		return -1;
	}
	

	refresh();
	wrefresh(log_win->main);

	int ch;
	int i = 0;

	// TODO: move to function
	// send nickname to server
	while(1) {
		touchwin(log_win->main);

		mvwprintw(log_win->sub, 1, 1, "Nickname: %s", nickname);
		wrefresh(log_win->sub);

		//ch = getch();
		ch = wgetch(log_win->sub);

		if (ch != ERR) {
			if (ch == '\n' || ch == '\r') {
				nickname[i] = '\n';

				mvwprintw(log_win->sub, 1, 1, "Nickname: %s", nickname);
				wrefresh(log_win->sub);

				if(strlen(nickname) < 3) {
					err_screen(NULL,NULL,"Nickname too short (2 - 15 characters.)");
					i = 0;
					memset(nickname, 0, sizeof(nickname));
					wmove(log_win->sub, 1,1);
					wclrtoeol(log_win->sub); // clear line to end
					mvwprintw(log_win->sub, 1, 1, "Nickname: %s", nickname);
					wrefresh(log_win->sub);
					wrefresh(log_win->main);
				}
				else if (strlen(nickname) > 15) {
					err_screen(NULL,NULL,"Nickname too long (2 - 15 characters.)");
					i = 0;
					memset(nickname, 0, sizeof(nickname));
					wmove(log_win->sub, 1,1);
					wclrtoeol(log_win->sub); // clear line to end
					mvwprintw(log_win->sub, 1, 1, "Nickname: %s", nickname);
					wrefresh(log_win->sub);
					wrefresh(log_win->main);
				} 
				else {
					int ERR_send = send(SOCK_FileDiscriptor, nickname, sizeof(nickname), 0);
					if(ERR_send == -1) {
						err_screen(NULL, NULL,"Error, could not send nickname.\n");
					}
					break;
				}
			}
			else if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
				if (i > 0) { 
					i--;
					nickname[i] = ' ';
					wclrtoeol(log_win->sub);
					mvwprintw(log_win->sub, 1, 1, "Nickname: %s", nickname);
					nickname[i] = '\0';
					//wmove(log_win->sub, 1, 1);
					wrefresh(log_win->sub);
				}
				else {
					i=0;
				}
			}
			else {
				nickname[i] = ch;
				wmove(log_win->sub, 1, 1);
				wrefresh(log_win->sub);
				i++;
			}
		}
	}

	// remove \n 
	nickname[strlen(nickname) -1] = '\0';

	fd_set fd_bitmap;
	int sock_fd = SOCK_FileDiscriptor;

	// TODO: change to nested window
	struct Win_nested *send_win = NULL;
	send_win = win_nested(nickname, 4, rs_col, rs_row-4, 0, 0);
	if (send_win == NULL) {
		err_screen(NULL, NULL,"Error creating send window\n");
		return -1;
	}

	// create win+sub_window
	struct Win_nested *recv_win = NULL;
	recv_win = win_nested("Chatroom", rs_row-4, rs_col,0,0,0);
	if (send_win == NULL) {
		err_screen(NULL, NULL,"Error creating recieve window\n");
		return -1;
	}

	// disable cursor
	curs_set(0);

	// TODO: might not be needed for all
	nodelay(send_win->sub, TRUE);
	nodelay(recv_win->main, TRUE);
	nodelay(recv_win->sub, TRUE);

	win_reset(send_win, recv_win);

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

	// TODO: move to seperate function
	while (1) {
		// refresh boxes
		touchwin(stdscr);
		touchwin(send_win->main);
		touchwin(recv_win->main);

		// Redraw prompt and current input
		mvwprintw(send_win->sub, 0, 0, "%s: %s", nickname, message);
		win_reset(send_win, recv_win);

		// test error function
		if (!test) {
			err_screen(NULL, "TEST ERR","test error");
			test = 1;
		}

		ch = wgetch(send_win->sub);

		if (ch != ERR) {
			if (ch == '\n' || ch == '\r') {
				message[i] = '\n';

				// TODO put into function/ struct with other commands
				if (!strcmp(message, "/quit\n")) {
					close(SOCK_FileDiscriptor);
					break;
				}
				// if message is just enter, do nothing
				else if ( message[0] == '\n') {
					continue;
				}

				// TODO instead of sending raw text buffer, an type struct should be only send/ recieved
				// so one recv/send function can parse multiple types of signals/ requests
				int ERR_send = send(SOCK_FileDiscriptor, message, strlen(message), 0);
				if(ERR_send == -1) {
					err_screen(NULL, NULL,"Error, could not send message.\n");
				}

				i = 0;
				memset(message, 0, sizeof(message));
				wmove(send_win->sub, 0,1);
				wclrtoeol(send_win->sub); // clear line to end
				mvwprintw(send_win->sub, 0, 0, "%s: %s", nickname, message);
				win_reset(send_win, recv_win);
			}
			else if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
				// reprint, removing character and set to null
				// otherwise user can send deleted string as sequence of empty spaces
				if (i > 0) {
					i--;
					message[i] = ' ';
					mvwprintw(send_win->sub, 0, 0, "%s: %s", nickname, message);
					message[i] = '\0';
				}
				else {
					i=0;
				}
			}
			else {
				message[i] = ch;
				wmove(send_win->sub, 0, 0);
				wrefresh(send_win->sub);
				wrefresh(send_win->main);
				i++;
			}
		}

		char r_msg[256];
		int client_quit = recv(SOCK_FileDiscriptor, r_msg, sizeof(r_msg), 0);
		if (client_quit > 0 && r_msg[0] != '\0') {
			// TODO: add timestamp and nick of sender
			wprintw(recv_win->sub, "Recieved: %s",r_msg);
			touchwin(recv_win->main);
			win_reset(send_win, recv_win);
			r_msg[0] = '\0';
		} 
		else if (client_quit == 0) {
			err_screen(NULL,NULL,"Server closed connection");
			break;

		}
		else {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				// No data now — continue looping
				continue;
			} 
			else {
				// Real error
				err_screen(NULL,NULL,"Error sending message to server");
				break;
			}
		}
		usleep(10000);
	}

	endwin(); // end curses mode
	
	// deallocate heap mem
	if ( recv_win != NULL ) {
		free(recv_win);
		recv_win = NULL;
	}

	if ( send_win != NULL ) {
		free(send_win);
		send_win = NULL;
	}

	if ( log_win != NULL ) {
		free(log_win); // TODO: maybe better deallocate after nickname was send
		log_win = NULL;
	}

	close(SOCK_FileDiscriptor);
	return 0;
}
