#include "utils.h"
#include <time.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>


struct tm* 
timestamp() 
{
	time_t login_timestamp;
	time(&login_timestamp);
	struct tm *ts = localtime(&login_timestamp);
	return ts;
}

void
win_errpopup( WINDOW *p_window, char *win_title, char *err_msg) 
{
	if (p_window == NULL) {
		p_window = stdscr;
	}

	// souce/ root window diameters
	int wy_max;
	int wx_max;

	int wy_mid;
	int wx_mid;

	// popup window
	int popy_max;
	int popx_max;

	int popy_mid;
	int popx_mid;

	int pop_height;
	int pop_width;
	int popy_start;
	int popx_start;

	int err_msg_len;

	// get length and width of terminal/ or given window
	getmaxyx(p_window, wy_max, wx_max);

	// popup window diameters
	pop_height = 8;
	pop_width = wx_max - 10;
	popx_start = 5;

	// calculate center of window y, x
	popy_start = (wy_max/2) - (pop_height/2);
	wx_mid = wx_max/2;

	// create new popup window
	WINDOW *p_pop_win;
	p_pop_win = newwin(pop_height, pop_width, popy_start, popx_start);
	box(p_pop_win, 0,0);

	if (win_title != NULL) {
		mvwprintw(p_pop_win, 0, 1, "%s", win_title);
	}

	getmaxyx(p_pop_win, popy_max, popx_max);
	
	// get middle point of popup win
	popy_mid = popy_max/2;
	popx_mid = popx_max/2;

	err_msg_len = strlen(err_msg);

	mvwprintw(p_pop_win, popy_mid-1, popx_mid - err_msg_len/2, "%s",err_msg);

	// print in reverse colors
	wattron(p_pop_win,A_REVERSE);
	mvwprintw(p_pop_win, popy_mid+1, popx_mid-3, "> OK <");
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

struct Win_nested*
win_nested(char *title, int winy, int winx, int drawpty, int drawptx, int wfl) 
{
	// disable echoing of keyboard shortcuts
	noecho();

	// rs - root screen
	int rs_row;
	int rs_col;

	getmaxyx(stdscr,rs_row,rs_col);
	
	// calloc both windows/ or struct
	WINDOW *main_win;
	WINDOW *sub_win;

	//floating window option
	if (wfl == 1) {
		// set draw point of window
		drawpty = rs_row/2-winy/2;
		drawptx = rs_col/2-winx/2;
	}

	// create window w/ sub window
	main_win  = newwin(winy, winx, drawpty, drawptx);
	sub_win = derwin(main_win, winy-2, winx-2, 1, 1);

	// enable scrolling
	scrollok(sub_win, TRUE);

	struct Win_nested *wn;
	wn = calloc(1, sizeof(struct Win_nested));

	wn->main = main_win;
	wn->sub = sub_win;

	box(main_win,0,0);

	// print window title
	if (title != NULL) {
		mvwprintw(main_win,0,1, "%s", title);
	}

	return wn;
}

void
win_reset(struct Win_nested *wn1, struct Win_nested *wn2)
{
	wrefresh(wn1->main);
	wrefresh(wn1->sub);
	wrefresh(wn2->main);
	wrefresh(wn2->sub);
}

void
win_ui_init(struct Win_ui *ui)
{
	// create login window
	int rs_row;
	int rs_col;
	int log_winy = 10;
	int log_winx = 40;
	getmaxyx(stdscr,rs_row,rs_col);

	ui->send_win = win_nested(ui->nickname, 4, rs_col, rs_row-4, 0, 0);
	if (ui->send_win == NULL) {
		win_errpopup(NULL, NULL,"Error creating send window\n");
	}

	ui->recv_win = win_nested("Chatroom", rs_row-4, rs_col, 0, 0, 0);
	if (ui->send_win == NULL) {
		win_errpopup(NULL, NULL,"Error creating recieve window\n");
	}
};

void
win_login_init(struct Win_ui *ui)
{
	// create login window
	int rs_row;
	int rs_col;
	int log_winy = 10;
	int log_winx = 40;
	getmaxyx(stdscr,rs_row,rs_col);

	ui->login_win = win_nested(0, log_winy, log_winx,0,0, 1);
	if (ui->login_win == NULL) {
		win_errpopup(NULL, NULL,"Error creating login window\n");
	}
};

void
win_free(struct Win_ui *ui)
{
	// deallocate all heap mem
	if ( ui->recv_win != NULL ) {
		free(ui->recv_win);
		ui->recv_win = NULL;
	}

	if ( ui->send_win != NULL ) {
		free(ui->send_win);
		ui->send_win = NULL;
	}

	if ( ui->login_win != NULL ) {
		free(ui->login_win);
		ui->login_win = NULL;
	}
};


