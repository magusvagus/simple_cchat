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
err_screen( WINDOW *p_window, char *err_msg) 
{
	if (p_window == NULL) {
		p_window = stdscr;
	}

	int w_max_y;
	int w_max_x;

	int w_root_y;
	int w_root_x;

	int pop_max_y;
	int pop_max_x;

	int pop_root_y;
	int pop_root_x;

	int pop_height;
	int pop_width;
	int pop_start_x;
	int pop_start_y;

	int err_msg_len;

	// get length and width of terminal/ or given window
	getmaxyx(p_window, w_max_y, w_max_x);

	// popup window diameters
	pop_height = 8;
	pop_width = w_max_x - 10;
	pop_start_x = 5;

	// calculate root of window y, x
	pop_start_y = (pop_max_y/2) - (pop_height/2);
	w_root_x = w_max_x/2;


	WINDOW *p_pop_win;
	p_pop_win = newwin(pop_height, pop_width, pop_start_y, pop_start_x);
	box(p_pop_win, 0,0);

	getmaxyx(p_pop_win, pop_max_y, pop_max_x);
	
	pop_root_y = pop_max_y/2;
	pop_root_x = pop_max_x/2;

	err_msg_len = strlen(err_msg);

	mvwprintw(p_pop_win, pop_root_y-1, pop_root_x - err_msg_len/2, "%s",err_msg);

	// print in reverse colors
	wattron(p_pop_win,A_REVERSE);
	mvwprintw(p_pop_win, pop_root_y+1, pop_root_x-3, "> OK <");
	wattroff(p_pop_win,A_REVERSE);

	int ch;

	refresh();
	wrefresh(p_pop_win);

	// close window if input id given
	ch = wgetch(p_pop_win);
	wclear(p_pop_win);
	wrefresh(p_pop_win);
	delwin(p_pop_win);
}
