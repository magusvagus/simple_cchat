#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // inet_pton
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <ncurses.h>
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
	getmaxyx(stdscr,rs_row,rs_col);

	// set login win
	WINDOW *log_win;
	log_win = newwin(rs_row, rs_col, 0,0);
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

	WINDOW *recv_win;
	WINDOW *send_win;

	recv_win = newwin( rs_row - 4, rs_col, 0, 0);
	send_win = newwin(4, rs_col, rs_row - 4, 0);

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

	//int ch;
	int pos = 0;

	cbreak();
	nodelay(stdscr, TRUE);
	timeout(0);
	noecho();
	keypad(stdscr, TRUE);

	i = 0;
	int c = 1;

	int test = 0;

	// TODO: move to seperate function
	while (1) {
		// refresh boxes
		box(recv_win, 0,0);
		box(send_win, 0,0);

		// Redraw prompt and current input
		mvwprintw(send_win, 1, 1, "%s: %s", nickname, message);
		wrefresh(send_win);

		// test error function
		if (!test) {
			err_screen(NULL, "TEST ERR","test error");
			test = 1;
		}

		ch = getch();

		if (ch != ERR) {
			if (ch == '\n' || ch == '\r') {
				message[i] = '\n';

				// TODO put into function/ struct with other commands
				if (!strcmp(message, "/quit\n")) {
					close(SOCK_FileDiscriptor);
					break;
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
