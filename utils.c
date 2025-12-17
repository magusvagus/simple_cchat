#include "utils.h"
#include <time.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>


struct tm* 
timestamp() 
{
	time_t login_timestamp;
	time(&login_timestamp);
	struct tm *ts = localtime(&login_timestamp);
	return ts;
}

void
err_screen( WINDOW *window, char *err_msg) 
{
	if (window == NULL) {
		window = stdscr;
	}

	int w_max_y;
	int w_max_x;

	int w_center_y;
	int w_center_x;

	int errw_max_y;
	int errw_max_x;

	int errw_center_y;
	int errw_center_x;

	int err_msg_len;

	// get length and width of terminal/ window
	getmaxyx(window, w_max_y, w_max_x);

	w_center_y = w_max_y/2;
	w_center_x = w_max_x/2;

	WINDOW *err_window;
	err_window = newwin(w_center_y - 10, w_max_x - 10, w_center_y, 5);
	box(err_window, 0,0);

	getmaxyx(err_window, errw_max_y, errw_max_x);
	
	errw_center_y = errw_max_y/2;
	errw_center_x = errw_max_x/2;

	err_msg_len = strlen(err_msg);

	mvwprintw(err_window, errw_center_y-1, errw_center_x - err_msg_len/2, "%s",err_msg);

	wattron(err_window,A_REVERSE);
	mvwprintw(err_window, errw_center_y+1, errw_center_x-3, "> OK <");
	wattroff(err_window,A_REVERSE);

	int ch;

	refresh();
	wrefresh(err_window);

	// close window if input id given
	ch = wgetch(err_window);
	wclear(err_window);
	wrefresh(err_window);
	delwin(err_window);
}
