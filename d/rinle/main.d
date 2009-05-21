module rinle.main;

import rinle.focus;
import rinle.model.interfaces;
import rinle.model.filesystem;
import rinle.view.view;
import rinle.view.ui;
//import rinle.commands;

import gubg.ncurses;
import gubg.patterns.command;
import gubg.patterns.chainOfResponsibility;

class Rinle
{
    this()
        {
	    puts("Starting Rinle");
            _nCurses = new NCurses;
	    _ui = new UI(_nCurses, _nCurses);

            _focusMgr = new FocusMgr(_ui);
	    _focusMgr.push(new Focus);

            _model = new Dir;
            _view = new View(_model);
        }
    ~this()
        {
	    puts("Stopping Rinle");
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
	    _view.show(_ui.output);
            _ui.output.refresh;
        }

    class Focus: IFocus
    {
        // IFocus
        void setUI(IUI ui)
        {
	    _ui = ui;
        }
        bool handle(inout ICommand command)
        {
            int key;
            switch (key = _ui.input.getKey)
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
            case 'i':
                command = new InsertCommand("end", _ui);
                break;
            case 'o':
                command = new InsertCommand("after", _ui);
                break;
            case 'u':
                command = new InsertCommand("before", _ui);
                break;
            case 'r':
                command = new InsertCommand("replace", _ui);
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
	IUI _ui;
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

    class InsertCommand: ICommand
        {
            this (char[] location, IUI ui)
            {
                _location = location;
		_ui = ui;
            }
            bool execute()
            {
                _view.insert(_location, _ui);
                return true;
            }
            bool undo(){return false;}
            char[] _location;
	    IUI _ui;
        }

    Dir _model;
    View _view;
    
    bool _proceed;

    NCurses _nCurses;
    IUI _ui;

    FocusMgr _focusMgr;
}

import gubg.puts;

import tango.core.Thread;

int main()
{
    putsFile("/tmp/rinle.output");

    scope rinle = new Rinle;

    rinle.run;

    return 0;
}

