#include <ncurses.h>

#ifndef UTILS_H
#define UTILS_H


struct Win_nested {
	WINDOW* main;
	WINDOW* sub;
};

struct Win_ui {
	struct Win_nested *send_win;
	struct Win_nested *recv_win;
	struct Win_nested *nick_win;
};

struct tm* timestamp();
void err_screen(WINDOW *p_window,char *win_title, char *err_msg);
struct Win_nested* win_nested(char *title, int winy, int winx, int drawpty, int drawptx, int wfl);
void win_reset(struct Win_nested *wn1, struct Win_nested *wn2);

#endif 
