module rinle.rinle;

import gubg.ui;
import gubg.ncurses;
import gubg.patterns.command;
import gubg.patterns.chainOfResponsibility;

import rinle.focus;
//import rinle.commands;

class Rinle: IChainOfResponsibility!(ICommand)
{
    this()
        {
            mNCurses = new NCurses;
            mInput = mNCurses;
            mOutput = mNCurses;

            mFocus = new Focus(mInput, mOutput);
	    mFocus.push(this);
        }
    ~this()
        {
            delete mNCurses;
	    mFocus.pop;
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
            mFocus.handle(command);
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
                command = new QuitCommand;
                break;
            default:
		// This is the last level, we don't try to delegate to our successor
                return false;
                break;
            }
            return true;
        }
    void successor(IChainOfResponsibility!(ICommand) handler){};

    void quit()
        {
            mProceed = false;
        }

private:
    class QuitCommand: ICommand
    {
	bool execute()
	{
	    quit();
	    return true;
	}
	bool undo(){return false;}
    }

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

