module rinle.focus;

import gubg.ui;
import gubg.patterns.command;
import gubg.patterns.chainOfResponsibility;

class Focus: ChainOfResponsibility!(ICommand)
{
    this(Input input, Output output)
        {
            mInput = input;
            mOutput = output;
        }

private:
    Input mInput;
    Output mOutput;
}
