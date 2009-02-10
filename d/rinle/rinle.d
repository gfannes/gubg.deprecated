module rinle.rinle;

import gubg.ui;
import gubg.ncurses;
import gubg.patterns.command;
import gubg.patterns.chainOfResponsibility;

import rinle.focus;
import rinle.commands;

class Rinle: IChainOfResponsibility!(ICommand)
{
    this()
        {
            mNCurses = new NCurses;
            mInput = mNCurses;
            mOutput = mNCurses;

            mFocus = new Focus(mInput, mOutput);
        }
    ~this()
        {
            delete mNCurses;
        }

    void run()
        {
            mProceed = true;
            while (mProceed)
            {
                show;
                ICommand command = getCommand;
                if (command !is null)
                    command.execute;
            }
        }

    ICommand getCommand()
        {
            ICommand command;
//            mFocus.handle(command);
            handle(command);
            return command;
        }

    void show()
        {
            mOutput.refresh;
        }

    bool handle(inout ICommand command)
        {
            int key;
            switch (key = mInput.getKey)
            {
            case 'q':
                command = new QuitCommand(&quit);
                break;
            default:
                return false;
                break;
            }
            return true;
        }

    void quit()
        {
            mProceed = false;
        }

private:
    // root, base, current
    
    bool mProceed;

    NCurses mNCurses;
    Input mInput;
    Output mOutput;

    Focus mFocus;
}

import tango.core.Thread;

int main()
{
    scope rinle = new Rinle;

    rinle.run;

    return 0;
}

