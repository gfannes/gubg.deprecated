module rinle.view.ui;

import rinle.model.interfaces;
import gubg.ui;

class UI: IUI
{
    this(Input input, Output output)
    {
        _input = input;
        _output = output;
    }
    bool getString(inout char[] str, char[] msg)
    {
        _output.print(msg, _output.height/2, (_output.width-msg.length)/2);
        _output.refresh;
        bool finished = false;
        bool inputOK = true;
        bool escPressed = false;
        while (!finished)
        {
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

    bool selectString(inout uint ix, char[] msg, char[][] options)
    {
        _output.print(msg, 1, (_output.width-msg.length)/2);
        _output.refresh;
        bool finished = false;
        bool inputOK = false;
        bool escPressed = false;
        char[] str;
        while (!finished)
        {
            char[][] validOptions = filterOptions(options, str);
            printOptionsAndInput(validOptions, str);
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
                    finished = true;
                    break;
                case Key.backspace:
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


private:
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

    void printInput(char[] str)
    {
        printBlank(_output.height/2+1);
        _output.print(str, _output.height/2 + 1, (_output.width-str.length)/2);
        _output.refresh;
    }

    char[][] filterOptions(char[][] options, char[] str)
    {
        return options;
    }
    void printOptionsAndInput(char[][] options, char[] str)
    {
        foreach (ix, option; options)
        {
            _output.print(option, 1 + ix, 1);
        }
        printBlank(1 + options.length);
        _output.print(str, 1 + options.length, (_output.width-str.length)/2);
        _output.refresh;
    }
    
    Input _input;
    Output _output;    
}

version (UnitTest)
{
    import gubg.puts;
    import gubg.ncurses;
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
                ok = ui.selectString(ix, "Please select a string:", ["A", "a", "AA", "BBBBBB"]);
            }
            puts("You selected \"{}\", ok = {}", ix, ok);
        }
        uint ix;
    }
}
