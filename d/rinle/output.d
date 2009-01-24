module rinle.output;

import tango.core.Thread;

public import gubg.ncurses;
import rinle.line;
import gubg.puts;

class TextRectangle
{
    this(NCurses ncurses)
	{
	    mNCurses = ncurses;
	    mNCurses.createColor(0, NCurses.white, NCurses.black);
	    mNCurses.createColor(1, NCurses.black, NCurses.white);
	    mLines.length = height;
	    for (int i = 0; i < height; ++i)
		mLines[i] = new Line();
	    mLogThreads = new ThreadGroup;
	}
    ~this()
	{
	    mLogThreads.joinAll;
	}

    int width(){return mNCurses.width;}
    int height(){return mNCurses.height();}

    bool display(Line[] lines)
	{
	    for (int ix = 0; ix < height; ++ix)
	    {
		if (ix < lines.length)
		    mLines[ix] = lines[ix];
		else
		    mLines[ix].reset();
		mLines[ix].length = width;
		mLines[ix].each(delegate void(char[] str, int colIX, short color)
				{
				    mNCurses.print(str, ix, colIX, color);
				});
	    }
	    mNCurses.update;
	    return true;
	}
    bool restoreDisplay()
	{
	    for (int ix = 0; ix < height; ++ix)
		restoreLine(ix);
	    mNCurses.update;
	    return true;
	}
    void restoreLine(uint ix)
	{
	    mLines[ix].length = width;
	    mLines[ix].each(delegate void(char[] str, int colIX, short color)
			    {
				mNCurses.print(str, ix, colIX, color);
			    });
	}

    bool fill(char ch)
	{
	    char[] str;
	    str.length = width;
	    for (int ix = 0; ix < width; ++ix)
		str[ix] = ch;
	    for (int ix = 0; ix < height; ++ix)
	    {
		mLines[ix].reset();
		mLines[ix].add(str, 0);
	    }
	    return display(mLines);
	}

    bool popup(char[][] lines)
	{
	    uint nrLines = lines.length;
	    if (nrLines == 0)
		return false;
	    if (nrLines > height)
		nrLines = height;
	    int offsetRow = (height - (nrLines));
	    if (offsetRow > 0)
		offsetRow /= 2;
	    else
		offsetRow = 0;

	    uint maxW = lines[0].length;
	    foreach(line; lines)
		if (line.length > maxW)
		    maxW = line.length;
	    int offsetCol = (width - (maxW));
	    if (offsetCol > 0)
		offsetCol /= 2;
	    else
		offsetCol = 0;

	    char[] line;
	    line.length = maxW;
	    for (int ix = 0; ix < nrLines; ++ix)
	    {
		uint len = lines[ix].length;
		line[0 .. len] = lines[ix];
		line.length = maxW;
		for (uint i = len; i < maxW; ++i)
		    line[i] = ' ';
		restoreLine(ix + offsetRow);
		mNCurses.print(line, ix + offsetRow, offsetCol, 0);
	    }
	    mNCurses.update;
	    return true;
	}

    bool message(char[] str, real time)
	{
	    char[] m;
	    int nr = (width - (str.length + 2));
	    if (nr > 0)
	    {
		nr /= 2;
		char[] fix;
		fix.length = nr+1;
		for (int i = 0; i < nr+1; ++i)
		    fix[i] = '*';
		m = fix[0 .. $-1] ~ " " ~ str ~ " " ~ fix;
	    } else
		m = str;
	    int ix = mNCurses.halfHeight;
	    mNCurses[ix] = m;
	    mNCurses.update;
	    Thread.sleep(time);
	    mLines[ix].each(delegate void(char[] str, int colIX, short color)
			    {
				mNCurses.print(str, ix, colIX, color);
			    });
	    mNCurses.update;
	    return true;
	}

    void logThread()
	{
	    real time;
	    synchronized(mLogThreads){time = mLogTime;}
	    Thread.sleep(time);
	    mLines[height-1].each(delegate void(char[] str, int colIX, short color)
			    {
				mNCurses.print(str, height-1, colIX, color);
			    });
	    mNCurses.update;
	}
    bool log(char[] str, real time = 0.5)
	{
	    mNCurses[height-1] = str;
	    mNCurses.update;
	    synchronized(mLogThreads)
	    {
		mLogTime = time;
	    }
	    mLogThreads.create(&logThread);
	    Thread[] stoppedThreads;
	    foreach (t; mLogThreads)
		if (!t.isRunning)
		    stoppedThreads ~= [t];
	    foreach (t; stoppedThreads)
		mLogThreads.remove(t);
	    return true;
	}

private:
    NCurses mNCurses;
    Line[] mLines;
    real mLogTime;
    ThreadGroup mLogThreads;
}
