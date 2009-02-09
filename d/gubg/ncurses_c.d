module gubg.ncurses_c;

alias void WINDOW;
alias uint chtype;

enum Color: short
{
    black = 0,
	red,
	green,
	yellow,
	blue,
	magenta,
	cyan,
	white
}

extern(C)
{
    WINDOW *initscr();
    int endwin();
    int mvaddch(int, int, chtype);
    int refresh();
    int cbreak();
    int noecho();
    int getch();
    int start_color();
    int init_pair(short, short, short);
    int COLOR_PAIR(int);
    int attron(int);
    int attroff(int);
    int nodelay(WINDOW*, bool);

    int LINES;
    int COLS;
}
