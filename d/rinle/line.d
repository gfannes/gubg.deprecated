module rinle.line;

class Style
{
    this (int eix, short col)
	{
	    endIX = eix;
	    color = col;
	}
    int endIX;
    short color;
}

class Line
{
    this ()
	{
	}
    void add(char[] str, short color)
	{
	    line ~= str;
	    styles ~= [new Style(line.length, color)];
	}
    void each(void delegate(char[] str, int colIX, short color) yield)
	{
	    int startIX = 0;
	    foreach (style; styles)
	    {
		yield(line[startIX .. style.endIX], startIX, style.color);
		startIX = style.endIX;
	    }
	    if (startIX < length)
	    {
		char[] str;
		str.length = length-startIX;
		foreach (inout ch; str)
		    ch = ' ';
		yield(str, startIX, 0);
	    }
	}
    void reset()
	{
	    line.length = 0;
	    styles.length = 0;
	}
    int length;
    char[] line;
    Style[] styles;
}
