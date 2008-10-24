module rinle.ui;

import gubg.ncurses;
import rinle.input;
import rinle.output;
import rinle.sink;
import rinle.line;
import gubg.puts;
import rinle.interfaces;

class UI
{
    this (Keyboard input = null, TextRectangle output = null)
	{
	    if (input is null && output is null)
	    {
		mNCurses = new NCurses;
		mInput = new Keyboard(mNCurses);
		mOutput = new TextRectangle(mNCurses);
	    } else
	    {
		mNCurses = null;
		mInput = input;
		mOutput = output;
	    }
	}
    ~this()
	{
	    if (mNCurses !is null)
	    {
		if (mInput)
		    delete mInput;
		if (mOutput)
		    delete mOutput;
		delete mNCurses;
	    }
	}

    Keyboard input(){return mInput;}

    int select(ref char[][] choices, int delegate(ref char[][] choices, int key) callback)
	{
	    int selection = -1;

	    do
	    {
		mOutput.popup(choices);
		selection = callback(choices, mInput.getKey());
	    } while (selection < 0);
	    mOutput.restoreDisplay;

	    return selection;
	}
    char[] askString(char[] prefix)
	{
	    char[] string;
	    char[][] choices = [prefix];
	    int choice = select(choices, delegate int(ref char[][] choices, int key)
				{
				    bool finish = false;
				    switch (key)
				    {
				    case 27: // escape
					string = "";
					break;
				    case 10: // enter
					finish = true;
					break;
				    default:
					string ~= [cast(char)(key)];
					break;
				    }
				    choices = [prefix ~ string];
				    return (finish ? 0 : -1);
				});
	    return string;
	}

    void display(Sink sink, INode current)
	{
	    Line[] buffer;
//	    sink.showAsTree;
	    sink.showNormal;
	    sink.collect(buffer);

	    uint h = mOutput.height;
	    int startIX = current.lineIX - h/3;
	    if (startIX < 0)
		startIX = 0;
	    for (int i = 0; i < h; ++i)
		buffer ~= [new Line];

	    mOutput.display(buffer[startIX .. startIX+h]);
	}

    void log(char[] l)
	{
	    mOutput.log(l);
	}

    void message(char[] m, real timeout)
	{
	    mOutput.message(m, timeout);
	}

private:
    NCurses mNCurses;
    Keyboard mInput;
    TextRectangle mOutput;
}
