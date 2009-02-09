module rinle.input;

public import gubg.ncurses;

class Keyboard
{
    this (NCurses ncurses)
	{
	    mNCurses = ncurses;
	}
    ~this ()
	{
	}

    int getKey(){return mNCurses.getKey;}
    void clearKeyBuffer(){mNCurses.clearKeyBuffer;}

private:
    NCurses mNCurses;
}
