module rinle.controller.UI;

//import rinle.model.Interfaces;
import gubg.UI;

// interface IUI
// {
//     bool getString(inout char[] str, char[] msg);
//     bool selectString(inout uint ix, char[] msg, char[][] options, bool confirm);
//     IInput input();
//     IOutput output();
// }

class UI//: IUI
{
    this(IInput input, IOutput output)
    {
        _input = input;
        _output = output;
    }

    IInput input(){return _input;}
    IOutput output(){return _output;}

    bool getString(inout char[] str, char[] msg)
    {
	scope sp = new SavePoint;
        bool finished = false;
        bool inputOK = true;
        bool escPressed = false;
        while (!finished)
        {
	    clear;
	    _output.print(msg, _output.height/2, (_output.width-msg.length)/2);
	    _output.refresh;
            printInput(str);
            auto key = _input.getKey;
            if (key == Key.esc)
            {
                // If esc is pressed twice, we exit without valid input
                if (escPressed)
                {
                    str = "";
                    inputOK = false;
                    finished = true;
                } else
                    str = "";
                escPressed = true;
            } else
            {
                escPressed = false;
                switch (key)
                {
                case Key.enter:
                    finished = true;
                    break;
                case Key.backspace:
                    if (str.length > 0)
                        str.length = str.length-1;
                    break;
                default:
                    str ~= [key];
                    break;
                }
            }
        }
        return inputOK;
    }

    bool selectString(inout uint ix, char[] msg, char[][] options, bool confirm)
    {
	scope sp = new SavePoint;
        bool finished = false;
        bool inputOK = false;
        bool escPressed = false;
        bool enterPressed = false;
        char[] str;
        // We start with all options valid
        char[][] validOptions = options;
        do
        {
            printMsgOptionsAndInput(msg, validOptions, str);
            auto key = _input.getKey;
            if (key == Key.esc)
            {
                // If esc is pressed twice, we exit without valid input
                if (escPressed)
                {
                    ix = 0;
                    inputOK = false;
                    finished = true;
                } else
                {
                    foreach (inout ch; str)
                        ch = ' ';
                    printInput(str);
                    str = "";
                    printInput(str);
                }
                escPressed = true;
            } else
            {
                escPressed = false;
                switch (key)
                {
                case Key.enter:
                    enterPressed = true;
                    break;
                case Key.backspace:
                    str.length = str.length-1;
                    break;
                default:
                    str ~= [key];
                    break;
                }
            }

            // Re-evaluate our options and exit if possible
            validOptions = filterOptions(options, str);
            if (enterPressed || (!confirm && validOptions.length == 1))
            {
                if (validOptions.length == 1)
                {
                    for (ix = 0; ix < options.length; ++ix)
                        if (options[ix] == validOptions[0])
                            break;
                    if (ix == options.length)
                        throw new Exception("Could not find the selected option.");
                    inputOK = true;
                }
                finished = true;
            }
        } while (!finished);

        return inputOK;
    }

private:
    scope class SavePoint
    {
	this()
	    {
		_savePoint = _output.save;
	    }
	~this()
	    {
		_savePoint.restore;
		delete _savePoint;
	    }

    private:
	IOutput.ISavePoint _savePoint;
    }

    void printBlank(uint rowIX)
    {
        static char[] blankLine;
        if (blankLine.length != _output.width)
        {
            blankLine.length = _output.width;
            foreach (inout ch; blankLine)
                ch = ' ';
        }
        _output.print(blankLine, rowIX, 0);
    }
    void clear()
    {
	for (uint rowIX = 0; rowIX < _output.height; ++rowIX)
	    printBlank(rowIX);
    }

    void printInput(char[] str)
    {
        printBlank(_output.height/2+1);
        _output.print(str, _output.height/2 + 1, (_output.width-str.length)/2);
        _output.refresh;
    }

    char[][] filterOptions(char[][] options, char[] str)
    {
	char[][] ret;
	foreach (option; options)
	    if (str.length <= option.length && str == option[0 .. str.length])
		ret ~= [option];
        return ret;
    }
    void printMsgOptionsAndInput(char[] msg, char[][] options, char[] str)
    {
	clear;
	_output.print(msg, 1, (_output.width-msg.length)/2);
        foreach (ix, option; options)
        {
            _output.print(option, 2 + ix, 1);
        }
        printBlank(2 + options.length);
        _output.print(str, 2 + options.length, (_output.width-str.length)/2);
        _output.refresh;
    }
    
    IInput _input;
    IOutput _output;    
}

version (UnitTest)
{
    import gubg.Puts;
    import gubg.NCurses;
    import tango.text.convert.Format;

    void main()
    {
        if (false)
        {
            bool ok;
            char[] str;
            {
                scope ncurses = new NCurses;
                auto ui = new UI(ncurses, ncurses);
                ok = ui.getString(str, "Please give me a string:");
            }
            puts("You gave me \"{}\", ok = {}", str, ok);
        }
        
        if (true)
        {
            bool ok;
            uint ix;
            {
                scope ncurses = new NCurses;
                auto ui = new UI(ncurses, ncurses);
                ok = ui.selectString(ix, "Please select a string:", ["A", "a", "AA", "BBBBBB"], false);
            }
            puts("You selected \"{}\", ok = {}", ix, ok);
        }
        uint ix;
    }
}