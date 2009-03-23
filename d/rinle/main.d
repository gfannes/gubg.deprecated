module rinle.main;

import gubg.ui;
import gubg.ncurses;
import gubg.patterns.command;
import gubg.patterns.chainOfResponsibility;

import rinle.focus;
import rinle.model.interfaces;
import rinle.model.filesystem;
import rinle.view.view;
//import rinle.commands;

class Rinle
{
    this()
        {
            mNCurses = new NCurses;
            mInput = mNCurses;
            mOutput = mNCurses;

            mFocusMgr = new FocusMgr(mInput, mOutput);
	    mFocusMgr.push(new Focus);

            mModel = new Dir;
            mView = new View(mModel);
        }
    ~this()
        {
            delete mNCurses;
	    mFocusMgr.pop;
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
            mFocusMgr.handle(command);
            return command;
        }

    void show()
        {
            mOutput.refresh;
        }

    class Focus: IFocus
    {
        // IFocus
        void setIO(Input input, Output output)
        {
            mInput = input;
            mOutput = output;
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

    private:
        Input mInput;
        Output mOutput;
    }

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

    Dir mModel;
    View mView;
    
    bool mProceed;

    NCurses mNCurses;
    Input mInput;
    Output mOutput;

    FocusMgr mFocusMgr;
}

import tango.core.Thread;

int main()
{
    scope rinle = new Rinle;

    rinle.run;

    return 0;
}

