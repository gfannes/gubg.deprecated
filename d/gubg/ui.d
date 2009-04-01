module gubg.ui;

import gubg.puts;

import tango.io.Stdout;

interface Input
{
    int getKey();
    void clearKeyBuffer();
}

enum Color: int
{
    black,
	red,
	green,
	yellow,
	blue,
	magenta,
	cyan,
	white
	}

struct ColorPair
{
    Color foreground;
    Color background;
}

const ColorPair defaultPair = {foreground: Color.white, background: Color.black};

interface Output
{
    int width();
    int height();
    bool opIndexAssign(char[] str, int lineIX);
    bool print(char[] str, int lineIX, int colIX, ColorPair pair = defaultPair);
    void refresh();
}

class StdOutput: Output
{
    this (int w = 114, int h = 30)
	{
	    mWidth = w;
	    mHeight = h;
	    mLines.length = mHeight;
	    foreach (inout line; mLines)
	    {
		line.length = mWidth;
		foreach (inout c; line)
		    c = ' ';
	    }
	}
    int width(){return mWidth;}
    int height(){return mHeight;}
    bool opIndexAssign(char[] str, int lineIX)
	{
	    if (lineIX < 0 || lineIX >= mHeight)
		return false;
	    int len = min(mWidth, str.length);
	    mLines[lineIX][0..len] = str[0..len];
	    if (len < mWidth)
		foreach (inout c; mLines[lineIX][len..mWidth])
		    c = ' ';
	    return true;
	}

    bool print(char[] str, int lineIX, int colIX, ColorPair pair = defaultPair)
	{
	    if (lineIX < 0 || lineIX >= mHeight || colIX < 0)
		return false;
	    int len = min(mWidth-colIX, str.length);
	    if (len > 0)
		mLines[lineIX][colIX..(colIX+len)] = str[0..len];
	    return true;
	}
    void refresh()
	{
	    puts("");
	    foreach (line; mLines)
		puts(line);
	}

private:
    static int min(int a, int b)
	{
	    return (a < b ? a : b);
	}

    int mWidth;
    int mHeight;
    char[][] mLines;
}

version (UnitTest)
{
    void main()
    {
	auto output = new StdOutput;
	output[1] = "abc";
	output.print("ABC", 10, 20);
	char[] line;
	line.length = output.width;
	foreach (inout c; line)
	    c = '.';
	output[output.height-1] = line;
	output.refresh;
    }
}
