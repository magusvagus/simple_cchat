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

	char message[1024];
	char buffer[1024];
	char nickname[17];

	// ncurses
	initscr(); // start curses mode
	// rs - root screan
	// int rs_row;
	// int rs_col;
	// int log_winy = 10;
	// int log_winx = 40;
	// getmaxyx(stdscr,rs_row,rs_col);

	// struct Win_nested *log_win = NULL;
	// log_win = win_nested(0, log_winy, log_winx,0,0, 1);
	// if (log_win == NULL) {
	// 	win_errpopup(NULL, NULL,"Error creating login window\n");
	// 	return -1;
	// }

	struct Win_ui ui = {0};
	ui.nickname = nickname;
	win_login_init(&ui);

	refresh();
	wrefresh(ui.login_win->main);

	int ch;
	int i = 0;

	// TODO: move to function
	// send nickname to server
	while(1) {
		touchwin(ui.login_win->main);

		mvwprintw(ui.login_win->sub, 1, 1, "Nickname: %s", ui.nickname);
		wrefresh(ui.login_win->sub);

		//ch = getch();
		ch = wgetch(ui.login_win->sub);

		if (ch != ERR) {
			if (ch == '\n' || ch == '\r') {
				ui.nickname[i] = '\n';

				mvwprintw(ui.login_win->sub, 1, 1, "Nickname: %s", ui.nickname);
				wrefresh(ui.login_win->sub);

				if(strlen(ui.nickname) < 3) {
					win_errpopup(NULL,NULL,"Nickname too short (2 - 15 characters.)");
					i = 0;
					memset(ui.nickname, 0, sizeof(ui.nickname));
					wmove(ui.login_win->sub, 1,1);
					wclrtoeol(ui.login_win->sub); // clear line to end
					mvwprintw(ui.login_win->sub, 1, 1, "Nickname: %s", ui.nickname);
					wrefresh(ui.login_win->sub);
					wrefresh(ui.login_win->main);
				}
				else if (strlen(ui.nickname) > 15) {
					win_errpopup(NULL,NULL,"Nickname too long (2 - 15 characters.)");
					i = 0;
					memset(ui.nickname, 0, sizeof(ui.nickname));
					wmove(ui.login_win->sub, 1,1);
					wclrtoeol(ui.login_win->sub); // clear line to end
					mvwprintw(ui.login_win->sub, 1, 1, "Nickname: %s", ui.nickname);
					wrefresh(ui.login_win->sub);
					wrefresh(ui.login_win->main);
				} 
				else {
					int ERR_send = send(SOCK_FileDiscriptor, ui.nickname, sizeof(ui.nickname), 0);
					if(ERR_send == -1) {
						win_errpopup(NULL, NULL,"Error, could not send nickname.\n");
					}
					break;
				}
			}
			else if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
				if (i > 0) { 
					i--;
					ui.nickname[i] = ' ';
					wclrtoeol(ui.login_win->sub);
					mvwprintw(ui.login_win->sub, 1, 1, "Nickname: %s", ui.nickname);
					ui.nickname[i] = '\0';
					//wmove(ui.login_win->sub, 1, 1);
					wrefresh(ui.login_win->sub);
				}
				else {
					i=0;
				}
			}
			else {
				ui.nickname[i] = ch;
				wmove(ui.login_win->sub, 1, 1);
				wrefresh(ui.login_win->sub);
				i++;
			}
		}
	}

	// remove \n 
	ui.nickname[strlen(ui.nickname) -1] = '\0';

	fd_set fd_bitmap;
	int sock_fd = SOCK_FileDiscriptor;

	win_ui_init(&ui);
	// struct Win_nested send_win = NULL;
	// send_win = win_nested(nickname, 4, rs_col, rs_row-4, 0, 0);
	// if (send_win == NULL) {
	// 	win_errpopup(NULL, NULL,"Error creating send window\n");
	// 	return -1;
	// }

	// create win+sub_window
	// struct Win_nested *ui.recv_win = NULL;
	// ui.recv_win = win_nested("Chatroom", rs_row-4, rs_col,0,0,0);
	// if (ui.send_win == NULL) {
	// 	win_errpopup(NULL, NULL,"Error creating recieve window\n");
	// 	return -1;
	// }

	// disable cursor
	curs_set(0);

	// TODO: might not be needed for all
	nodelay(ui.send_win->sub, TRUE);
	nodelay(ui.recv_win->main, TRUE);
	nodelay(ui.recv_win->sub, TRUE);

	win_reset(ui.send_win, ui.recv_win);

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
		touchwin(ui.send_win->main);
		touchwin(ui.recv_win->main);

		// Redraw prompt and current input
		mvwprintw(ui.send_win->sub, 0, 0, "%s: %s", nickname, message);
		win_reset(ui.send_win, ui.recv_win);

		// test error function
		if (!test) {
			win_errpopup(NULL, "TEST ERR","test error");
			test = 1;
		}

		ch = wgetch(ui.send_win->sub);

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
				// so one ui.recv/send function can parse multiple types of signals/ requests
				int ERR_send = send(SOCK_FileDiscriptor, message, strlen(message), 0);
				if(ERR_send == -1) {
					win_errpopup(NULL, NULL,"Error, could not send message.\n");
				}

				i = 0;
				memset(message, 0, sizeof(message));
				wmove(ui.send_win->sub, 0,1);
				wclrtoeol(ui.send_win->sub); // clear line to end
				mvwprintw(ui.send_win->sub, 0, 0, "%s: %s", nickname, message);
				win_reset(ui.send_win, ui.recv_win);
			}
			else if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
				// reprint, removing character and set to null
				// otherwise user can send deleted string as sequence of empty spaces
				if (i > 0) {
					i--;
					message[i] = ' ';
					mvwprintw(ui.send_win->sub, 0, 0, "%s: %s", nickname, message);
					message[i] = '\0';
				}
				else {
					i=0;
				}
			}
			else {
				message[i] = ch;
				wmove(ui.send_win->sub, 0, 0);
				wrefresh(ui.send_win->sub);
				wrefresh(ui.send_win->main);
				i++;
			}
		}

		char r_msg[256];
		int client_quit = recv(SOCK_FileDiscriptor, r_msg, sizeof(r_msg), 0);
		if (client_quit > 0 && r_msg[0] != '\0') {
			// TODO: add timestamp and nick of sender
			wprintw(ui.recv_win->sub, "Recieved: %s",r_msg);
			touchwin(ui.recv_win->main);
			win_reset(ui.send_win, ui.recv_win);
			r_msg[0] = '\0';
		} 
		else if (client_quit == 0) {
			win_errpopup(NULL,NULL,"Server closed connection");
			break;

		}
		else {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				// No data now — continue looping
				continue;
			} 
			else {
				// Real error
				win_errpopup(NULL,NULL,"Error sending message to server");
				break;
			}
		}
		usleep(10000);
	}

	endwin(); // end curses mode
	
	// deallocate heap mem
	win_free(&ui);

	close(SOCK_FileDiscriptor);
	return 0;
}
