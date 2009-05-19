module gubg.ui;

import gubg.puts;

import tango.io.Stdout;

interface IInput
{
    int getKey();
    void clearKeyBuffer();
}

enum Key: int
{
    enter = 10,
        esc = 27,
        left = 68,
        right = 67,
        up = 65,
        down = 66,
        tab = 9,
        backspace = 127,
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

interface IOutput
{
    interface ISavePoint
    {
	void restore();
    }
    int width();
    int height();
    bool opIndexAssign(char[] str, int lineIX);
    bool print(char[] str, int lineIX, int colIX, ColorPair pair = defaultPair);
    void refresh();
    ISavePoint save();
}

class StdOutput: IOutput
{
    this (int w = 114, int h = 30)
	{
	    _width = w;
	    _height = h;
	    _lines.length = _height;
	    foreach (inout line; _lines)
	    {
		line.length = _width;
		foreach (inout c; line)
		    c = ' ';
	    }
	}
    int width(){return _width;}
    int height(){return _height;}
    bool opIndexAssign(char[] str, int lineIX)
	{
	    if (lineIX < 0 || lineIX >= _height)
		return false;
	    int len = min(_width, str.length);
	    _lines[lineIX][0..len] = str[0..len];
	    if (len < _width)
		foreach (inout c; _lines[lineIX][len.._width])
		    c = ' ';
	    return true;
	}

    bool print(char[] str, int lineIX, int colIX, ColorPair pair = defaultPair)
	{
	    if (lineIX < 0 || lineIX >= _height || colIX < 0)
		return false;
	    int len = min(_width-colIX, str.length);
	    if (len > 0)
		_lines[lineIX][colIX..(colIX+len)] = str[0..len];
	    return true;
	}
    void refresh()
	{
	    puts("");
	    foreach (line; _lines)
		puts(line);
	}

    class SavePoint: ISavePoint
    {
	this(char[][] lines)
	{
	    _lines.length = lines.length;
	    foreach (ix, inout line; _lines)
	    {
		line.length = lines[ix].length;
		line[] = lines[ix][];
	    }
	}
	void restore()
	{
	    this.outer._lines.length = _lines.length;
	    foreach (ix, inout line; this.outer._lines)
	    {
		line.length = _lines[ix].length;
		line[] = _lines[ix][];
	    }
	}

    private:
	char[][] _lines;
    }
    SavePoint save()
    {
	return new SavePoint(_lines);
    }

private:
    static int min(int a, int b)
	{
	    return (a < b ? a : b);
	}

    int _width;
    int _height;
    char[][] _lines;
}

version (UnitTest)
{
    void main()
    {
	auto output = new StdOutput;

	auto sp1 = output.save;

	output[1] = "abc";
	output.print("ABC", 10, 20);
	char[] line;
	line.length = output.width;
	foreach (inout c; line)
	    c = '.';
	output[output.height-1] = line;
	output.refresh;

	auto sp2 = output.save;

	sp1.restore;
	output.refresh;

	sp2.restore;
	output.refresh;
    }
}
