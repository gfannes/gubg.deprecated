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
        bool ok = true;
        while (ok)
        {
            auto key = _input.getKey;
            switch (key)
            {
            case Key.esc:
                str = "";
                return false;
                break;
            case Key.enter:
                ok = false;
                break;
            case Key.backspace:
                if (str.length > 0)
                {
                    str[$-1] = ' ';
                    printInput(" " ~ str);
                    str.length = str.length-1;
                }
                printInput(str);
                break;
            default:
                str ~= [key];
                printInput(str);
                break;
            }
        }
        return true;
    }

private:
    void printInput(char[] str)
    {
        _output.print(str, _output.height/2+1, (_output.width-str.length)/2);
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
        bool ok;
        char[] str;
        {
            scope ncurses = new NCurses;
            auto ui = new UI(ncurses, ncurses);
            ok = ui.getString(str, "Please give me a string:");
        }
        puts("You gave me \"{}\", ok = {}", str, ok);
    }
}
