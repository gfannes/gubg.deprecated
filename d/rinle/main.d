module rinle.main;

import rinle.model.interfaces;
import rinle.model.filesystem;
import rinle.controller.interfaces;
import rinle.controller.global;

import gubg.ncurses;
import gubg.patterns.chainOfResponsibility;
import gubg.puts;

import tango.core.Thread;

class Rinle
{
    this()
        {
	    puts("Starting Rinle");
            _nCurses = new NCurses;
            _model = new Dir;
	    _controller = new GlobalController(_nCurses, _nCurses, _model, &quit);
        }
    ~this()
        {
	    puts("Stopping Rinle");
	    delete _controller;
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
                    command.execute;
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
}

int main()
{
    putsFile("/tmp/rinle.output");

    scope rinle = new Rinle;

    rinle.run;

    return 0;
}

