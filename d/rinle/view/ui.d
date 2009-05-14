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
        return false;
    }

private:
    Input _input;
    Output _output;    
}

version (UnitTest)
{
    void main()
    {
        auto output = new StdOutput;
        Input input;
        auto ui = new UI(input, output);

        char[] str;
        ui.getString(str, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    }
}
