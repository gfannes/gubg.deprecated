module rinle.commands;

import gubg.patterns.command;

class QuitCommand: ICommand
{
    this (void delegate() quitFunction)
    {
        mQuitFunction = quitFunction;
    }

    bool execute()
    {
        if (mQuitFunction is null)
            return false;

        mQuitFunction();
        return true;
    }
    bool undo(){return false;}

private:
    void delegate() mQuitFunction;
}
