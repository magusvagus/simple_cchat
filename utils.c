#include "utils.h"
#include <time.h>
#include <ncurses.h>


struct tm* 
timestamp() 
{
	time_t login_timestamp;
	time(&login_timestamp);
	struct tm *ts = localtime(&login_timestamp);
	return ts;
}

void
err_screen() 
{
	initscr(); // start curses mode

	int rs_row;
	int rs_col;

	// get length and width of terminal
	getmaxyx(stdscr,rs_row,rs_col);

	WINDOW *err_window;
	err_window = newwin( (rs_row / 2) - 13, (rs_col / 2) - 9, (rs_row / 2), (rs_row / 2));
	box(err_window, 0,0);

	mvwprintw(err_window, 1, 1, "test error");
	int ch;

	refresh();
	wrefresh(err_window);
	while (1) {
		ch = getch();
		if (ch != ERR) {
			delwin(err_window);
			break;
		}
	}
}
