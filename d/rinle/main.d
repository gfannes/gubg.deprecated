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
            _nCurses = new NCurses;
            _input = _nCurses;
            _output = _nCurses;

            _focusMgr = new FocusMgr(_input, _output);
	    _focusMgr.push(new Focus);

            _model = new Dir;
            _view = new View(_model);
        }
    ~this()
        {
            delete _nCurses;
	    _focusMgr.pop;
        }

    void run()
        {
            _proceed = true;
            while (_proceed)
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
            _focusMgr.handle(command);
            return command;
        }

    void show()
        {
	    _view.show(_output);
            _output.refresh;
        }

    class Focus: IFocus
    {
        // IFocus
        void setIO(Input input, Output output)
        {
            _input = input;
            _output = output;
        }
        bool handle(inout ICommand command)
        {
            int key;
            switch (key = _input.getKey)
            {
            case 'q':
                command = new QuitCommand;
                break;
            case 'w':
                command = new MoveCommand("up");
                break;
            case 's':
                command = new MoveCommand("down");
                break;
            case 'a':
                command = new MoveCommand("out");
                break;
            case 'd':
                command = new MoveCommand("in");
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
        Input _input;
        Output _output;
    }

    void quit()
        {
            _proceed = false;
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

    class MoveCommand: ICommand
        {
            this (char[] dir)
            {
                _dir = dir;
            }
            bool execute()
            {
                _view.move(_dir);
                return true;
            }
            bool undo(){return false;}
            char[] _dir;
        }

    Dir _model;
    View _view;
    
    bool _proceed;

    NCurses _nCurses;
    Input _input;
    Output _output;

    FocusMgr _focusMgr;
}

import tango.core.Thread;

int main()
{
    scope rinle = new Rinle;

    rinle.run;

    return 0;
}

