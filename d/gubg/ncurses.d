module gubg.ncurses;

import gubg.puts;

class NCurses
{
    const short black   = 0;
    const short red     = 1;
    const short green   = 2;
    const short yellow  = 3;
    const short blue    = 4;
    const short magenta = 5;
    const short cyan    = 6;
    const short white   = 7;

    this()
	{
	    mWindow = initscr();
	    cbreak();
	    noecho();
	    start_color();
	}
    ~this()
	{
	    endwin();
	}
    int width(){return COLS;}
    int halfWidth(){return COLS/2;}
    int height(){return LINES;}
    int halfHeight(){return LINES/2;}

    bool opIndexAssign(char[] str, int lineIX)
	{
	    synchronized(this)
	    {
		if (lineIX < 0 || lineIX >= height)
		    return false;
		
		for (int colIX = 0; colIX < width; ++colIX)
		{
		    if (colIX < str.length)
			mvaddch(lineIX, colIX, str[colIX]);
		    else
			mvaddch(lineIX, colIX, ' ');
		}
	    }
	    return true;
	}

    bool print(char[] str, int lineIX, int colIX, short color)
	{
	    synchronized(this)
	    {
		if (lineIX < 0 || lineIX >= height)
		    return false;

		attron(COLOR_PAIR(color));
		for (int ix = 0; colIX < width && ix < str.length; ++colIX, ++ix)
		    mvaddch(lineIX, colIX, str[ix]);
		attroff(COLOR_PAIR(color));
	    }
	    return true;
	}
    void createColor(short color, short fg, short bg)
	{
	    init_pair(color, fg, bg);
	}

    int update()
	{
	    synchronized(this)
	    {
		return refresh();
	    }
	}

    int getKey(){return getch();}
    void clearKeyBuffer()
	{
	    nodelay(mWindow, true);
	    while (getKey != -1){}
	    nodelay(mWindow, false);
	}

private:
    WINDOW* mWindow;
}

alias void WINDOW;
alias uint chtype;

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
