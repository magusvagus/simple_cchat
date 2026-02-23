#include <ncurses.h>

#ifndef UTILS_H
#define UTILS_H


struct Win_nested {
	WINDOW* main;
	WINDOW* sub;
};

struct Win_ui {
	char *nickname;
	struct Win_nested *send_win;
	struct Win_nested *recv_win;
	struct Win_nested *login_win;
};

struct tm* timestamp();
void win_errpopup(WINDOW *p_window,char *win_title, char *err_msg);
struct Win_nested* win_nested(char *title, int winy, int winx, int drawpty, int drawptx, int wfl);
struct Win_nested* win_pad_nested(char *title, int winy, int winx, int drawpty, int drawptx, int wfl);
void win_reset(struct Win_ui *ui);
void win_main_ui_init(struct Win_ui *ui);
void win_login_ui_init(struct Win_ui *ui);
void win_free(struct Win_ui *ui);
void win_login_input(struct Win_ui *ui, int socket_fd);
void win_ui_input(struct Win_ui *ui, int socket_fd);
int win_command(int socket_fd,char *message);

#endif 
