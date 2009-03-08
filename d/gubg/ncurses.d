module gubg.ncurses;

import gubg.ui;

import NC = gubg.ncurses_c;

class NCurses: Input, Output
{
    static this()
    {
	buildColorMap;
    }

    this()
    {
	mWindow = NC.initscr();
	NC.cbreak();
	NC.noecho();
	NC.start_color();
    }
    ~this()
    {
	NC.endwin();
    }
    
    int width(){return NC.COLS;}
    int height(){return NC.LINES;}
    
    bool opIndexAssign(char[] str, int lineIX)
    {
	synchronized(this)
	{
	    if (lineIX < 0 || lineIX >= height)
		return false;
	    
	    for (int colIX = 0; colIX < width; ++colIX)
	    {
		if (colIX < str.length)
		    NC.mvaddch(lineIX, colIX, str[colIX]);
		else
		    NC.mvaddch(lineIX, colIX, ' ');
	    }
	}
	return true;
    }
    
    bool print(char[] str, int lineIX, int colIX, ColorPair color)
    {
	synchronized(this)
	{
	    if (lineIX < 0 || lineIX >= height)
		return false;

	    short colorPair = getColorPair(color.foreground, color.background);
	    NC.attron(NC.COLOR_PAIR(colorPair));
	    for (int ix = 0; colIX < width && ix < str.length; ++colIX, ++ix)
		NC.mvaddch(lineIX, colIX, str[ix]);
	    NC.attroff(NC.COLOR_PAIR(colorPair));
	}
	return true;
    }
    
    int refresh()
    {
	synchronized(this)
	{
	    return NC.refresh();
	}
    }
    
    int getKey(){return NC.getch();}
    void clearKeyBuffer()
    {
	NC.nodelay(mWindow, true);
	while (getKey != -1){}
	NC.nodelay(mWindow, false);
    }
    
private:
    short getColorPair(Color fg, Color bg)
    {
	short color;
        short[Color]* pMap = null;
        short* pColor = null;

        // Check if the color already exists
        pMap = (bg in sKnownColors);
        if (pMap !is null)
            pColor = (fg in *pMap);

        if (pColor is null)
        {
            // Color does not exist yet: create and save it
            color = sNextColor;
            ++sNextColor;
            NC.init_pair(color, map(fg), map(bg));
            sKnownColors[bg][fg] = color;
        } else
            color = *pColor;
        
	return color;
    }
    
    static void buildColorMap()
    {
	sColorMap[Color.black] = NC.Color.black;
	sColorMap[Color.red] = NC.Color.red;
	sColorMap[Color.green] = NC.Color.green;
	sColorMap[Color.yellow] = NC.Color.yellow;
	sColorMap[Color.blue] = NC.Color.blue;
	sColorMap[Color.magenta] = NC.Color.magenta;
	sColorMap[Color.cyan] = NC.Color.cyan;
	sColorMap[Color.white] = NC.Color.white;
        sNextColor = 1;
    }
    short map(Color color)
    {
	return sColorMap[color];
    }
    static short[Color] sColorMap;
    static short[Color][Color] sKnownColors;
    static short sNextColor;
    
    NC.WINDOW* mWindow;
}

version (UnitTest)
{
    import tango.text.convert.Format;
    int main()
    {
	scope ncurses = new NCurses;
	Input input = ncurses;
	Output output = ncurses;

	auto line = new char[output.width];
	foreach (inout ch; line)
	    ch = '-';
	output[0] = line;
	output[output.height-1] = line;

	static ColorPair cp;
	for (int i = 0; i <= Color.max-Color.min; ++i)
	{
	    for (int j = 0; j <= Color.max-Color.min; ++j)
	    {
		cp.foreground = cast(Color)(Color.min + i);
		cp.background = cast(Color)(Color.min + j);
		output.print("X", 2*i+1, 2*j+1, cp);
	    }
	}

// 	auto key = input.getKey;
// 	output.print(Format("Key = {} ", key), 2, 3, cp);

	input.getKey;
	return 0;
    }
}