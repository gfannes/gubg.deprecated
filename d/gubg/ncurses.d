module gubg.ncurses;

import gubg.ui;

import NC = gubg.bindings.ncurses;

class NCurses: IInput, IOutput
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
        _currentColorPair = getColorPair(Color.white, Color.black);
        // Initialize the lines and color buffers for SavePoint
        initializeSavePoint;
    }
    ~this()
    {
	NC.endwin();
    }
    
    int width(){return NC.COLS;}
    int height(){return NC.LINES;}
    
    bool opIndexAssign(char[] str, uint lineIX)
    {
	synchronized(this)
	{
	    if (lineIX >= height)
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
    
    bool print(char[] str, uint lineIX, uint colIX, ColorPair color = defaultPair)
    {
	synchronized(this)
	{
	    if (lineIX >= height)
		return false;

            scope cp = new UseColorPair(color.foreground, color.background);
	    for (int ix = 0; colIX < width && ix < str.length; ++colIX, ++ix)
		mvaddch(lineIX, colIX, str[ix]);
	}
	return true;
    }
    
    void refresh()
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

    class SavePoint: ISavePoint
    {
        this(uint[][] lines, short[][] colors)
        {
            _lines.length = lines.length;
            foreach (ix, inout line; _lines)
            {
                line.length = lines[ix].length;
                line[] = lines[ix][];
            }
            _colors.length = colors.length;
            foreach (ix, inout color; _colors)
            {
                color.length = colors[ix].length;
                color[] = colors[ix][];
            }
        }

	void restore()
	{
            foreach (lineIX, line; _lines)
            {
                auto cols = _colors[lineIX];
                foreach (colIX, ch; line)
                {
                    auto col = cols[colIX];
                    NC.attron(NC.COLOR_PAIR(col));
                    mvaddch(lineIX, colIX, ch);
                    NC.attroff(NC.COLOR_PAIR(col));
                }
            }
	}
    private:
        uint[][] _lines;
        short[][] _colors;
    }
    SavePoint save()
    {
	return new SavePoint(_currentLines, _currentColors);
    }
    
private:
    class UseColorPair
    {
        this(Color fg, Color bg)
            {
                _colorPair = getColorPair(fg, bg);
                _currentColorPair = _colorPair;
                NC.attron(NC.COLOR_PAIR(_colorPair));
            }
        ~this()
            {
                NC.attroff(NC.COLOR_PAIR(_colorPair));
            }
    private:
        short _colorPair;
    }
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
    
    void mvaddch(uint lineIX, uint colIX, uint ch)
    {
        if (lineIX < height && colIX < width)
        {
            NC.mvaddch(lineIX, colIX, ch);
            _currentLines[lineIX][colIX] = ch;
            _currentColors[lineIX][colIX] = _currentColorPair;
        }
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

    void initializeSavePoint()
    {
        _currentLines.length = height;
        foreach (inout line; _currentLines)
        {
            line.length = width;
            foreach (inout ch; line)
                ch = ' ';
        }
        _currentColors.length = height;
        foreach (inout colors; _currentColors)
        {
            colors.length = width;
            foreach (inout cp; colors)
                cp = _currentColorPair;
        }
    }
    uint[][] _currentLines;
    short[][] _currentColors;
    short _currentColorPair;
}

version (UnitTest)
{
    import tango.text.convert.Format;
    int main()
    {
	scope ncurses = new NCurses;
	IInput input = ncurses;
	IOutput output = ncurses;

        // Take a snapshot of the clean window
        auto sp1 = output.save;

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

        // Take a snapshot of the colored window
        auto sp2 = output.save;

	auto key = input.getKey;
	output.print(Format("Key = {} ", key), 2, 3, cp);
	input.getKey;

        // Put back the clean-window snapshot
        sp1.restore;
        output.refresh;
	input.getKey;

        // Put back the colored snapshot
        sp2.restore;
        output.refresh;
	input.getKey;

	return 0;
    }
}
