#include <ncurses.h>

#ifndef UTILS_H
#define UTILS_H

struct Win_nested {
	WINDOW* main;
	WINDOW* sub;
};

struct tm* timestamp();
void err_screen(WINDOW *p_window,char *win_title, char *err_msg);
struct Win_nested* win_nested(char *title, int winy, int winx, int drawpty, int drawptx, int wfl);

#endif 
