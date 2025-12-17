#include <ncurses.h>

#ifndef UTILS_H
#define UTILS_H

struct tm* timestamp();
void err_screen(WINDOW *window, char *err_msg);

#endif 
