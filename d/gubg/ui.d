module gubg.ui;

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
    int refresh();
}
