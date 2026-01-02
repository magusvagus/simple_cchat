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
	int log_winx = 20;
	getmaxyx(stdscr,rs_row,rs_col);

	// set login win
	WINDOW *log_win;
	// TODO: rename each argument for newwin
	log_win = newwin(log_winy, rs_col/2+log_winx/2, rs_row/2-log_winy/2, rs_col/2-log_winx);
	box(log_win,0,0);
	refresh();
	wrefresh(log_win);

	int ch;
	int i = 0;

	// TODO: move to function
	// send nickname to server
	while(1) {
		// refresh box
		box(log_win,0,0);

		mvwprintw(log_win, 1, 1, "Nickname: %s", nickname);
		wrefresh(log_win);

		ch = getch();

		if (ch != ERR) {
			if (ch == '\n' || ch == '\r') {
				nickname[i] = '\n';

				mvwprintw(log_win, 1, 1, "Nickname: %s", nickname);
				wrefresh(log_win);

				if(strlen(nickname) <= 1) {
					err_screen(NULL,NULL,"Nickname too short (2 - 15 characters.)");
					i = 0;
					memset(nickname, 0, sizeof(nickname));
					wmove(log_win, 1,1);
					wclrtoeol(log_win); // clear line to end
					mvwprintw(log_win, 1, 1, "Nickname: %s", nickname);
					wrefresh(log_win);
				}
				else if (strlen(nickname) > 15) {
					err_screen(NULL,NULL,"Nickname too long (2 - 15 characters.)");
					i = 0;
					memset(nickname, 0, sizeof(nickname));
					wmove(log_win, 1,1);
					wclrtoeol(log_win); // clear line to end
					mvwprintw(log_win, 1, 1, "Nickname: %s", nickname);
					wrefresh(log_win);
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
					mvwprintw(log_win, 1, 1, "Nickname: %s", nickname);
					nickname[i] = '\0';
					wmove(log_win, 1, 1);
					wrefresh(log_win);
				}
				else {
					i=0;
				}
			}
			else {
				nickname[i] = ch;
				wmove(log_win, 1, 1);
				wrefresh(log_win);
				i++;
			}
		}
	}

	// remove \n 
	nickname[strlen(nickname) -1] = '\0';

	fd_set fd_bitmap;
	int sock_fd = SOCK_FileDiscriptor;

	WINDOW *send_win;
	send_win = newwin(4, rs_col, rs_row - 4, 0);
	box(send_win, 0,0);

	// create win+sub_window
	struct Win_nested *wn = NULL;
	wn = win_nested("Chatroom", rs_row-4, rs_col, 0);

	// disable cursor
	curs_set(0);

	// TODO: might not be needed for all
	nodelay(send_win, TRUE);
	nodelay(wn->main, TRUE);
	nodelay(wn->sub, TRUE);

	refresh();
	wrefresh(send_win);
	wrefresh(wn->sub);

	//int ch;
	int pos = 0;

	// disable cursor
	curs_set(0);

	cbreak();
	nodelay(stdscr, TRUE);
	timeout(0);
	noecho();
	keypad(stdscr, TRUE);

	i = 0;
	int c = 1;

	int test = 0;
	int y = 1;

	// Set socket to non-blocking
	int flags = fcntl(SOCK_FileDiscriptor, F_GETFL, 0);
	fcntl(SOCK_FileDiscriptor, F_SETFL, flags | O_NONBLOCK);

	// TODO: move to seperate function
	while (1) {
		// refresh boxes
		touchwin(stdscr);
		touchwin(send_win);
		touchwin(wn->main);


		// Redraw prompt and current input
		mvwprintw(send_win, 1, 1, "%s: %s", nickname, message);
		wrefresh(send_win);

		// test error function
		if (!test) {
			err_screen(NULL, "TEST ERR","test error");
			test = 1;
		}

		ch = wgetch(wn->main);

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
				wmove(send_win, 1,1);
				wclrtoeol(send_win); // clear line to end
				mvwprintw(send_win, 1, 1, "%s: %s", nickname, message);
				wrefresh(send_win);
			}
			else if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
				// reprint, removing character and set to null
				// otherwise user can send deleted string as sequence of empty spaces
				if (i > 0) {
					i--;
					message[i] = ' ';
					mvwprintw(send_win, 1, 1, "%s: %s", nickname, message);
					message[i] = '\0';
				}
				else {
					i=0;
				}
			}
			else {
				message[i] = ch;
				wmove(send_win, 1, 1);
				wrefresh(send_win);
				i++;
			}
		}
			char r_msg[256];
			int client_quit = recv(SOCK_FileDiscriptor, r_msg, sizeof(r_msg), 0);
			if (client_quit > 0 && r_msg[0] != '\0') {
				// TODO: add timestamp and nick of sender
				wprintw(wn->sub, "Recieved: %s",r_msg);
				//mvwprintw(recv_win, y, 1, "%s: %s\n", nickname, r_msg);
				touchwin(wn->main);
				// redraw title
				//mvwprintw(recv_win, 0, 1, "Chatroom");
				wrefresh(wn->main);
				y++;
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
	close(SOCK_FileDiscriptor);
	return 0;
}
