module rinle.main;

import rinle.model.Interfaces;
import rinle.model.Filesystem;
import rinle.controller.Interfaces;
import rinle.controller.Global;

import gubg.NCurses;
import gubg.patterns.ChainOfResponsibility;
import gubg.Puts;

import tango.core.Thread;
import tango.util.container.CircularList;

class Rinle
{
    this()
        {
	    puts("Starting Rinle");
            _nCurses = new NCurses;
            _model = new Dir;
	    _commandHistory = new CircularList!(ICommand);
	    _controller = new GlobalController(_nCurses, _nCurses, _model, _commandHistory, &quit);
        }
    ~this()
        {
	    puts("Stopping Rinle");
	    delete _controller;
	    delete _commandHistory;
	    delete _model;
            delete _nCurses;
        }

    void run()
        {
            _proceed = true;
            while (_proceed)
            {
                _controller.show;
                ICommand command;
                if (_controller.getCommand(command))
		{
                    if (!command.execute)
			puts("Failed to execute command.");
		    else if (command.undoable)
			_commandHistory.append(command);
		    puts("command history is now {} long", _commandHistory.size);
		}
            }
        }
    
    void quit()
        {
            _proceed = false;
        }

private:
    bool _proceed;

    Dir _model;
    IController _controller;

    NCurses _nCurses;

    CircularList!(ICommand) _commandHistory;
}

int main()
{
    putsFile("/tmp/rinle.output");

    scope rinle = new Rinle;

    rinle.run;

    return 0;
}