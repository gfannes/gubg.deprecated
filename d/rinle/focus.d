module rinle.focus;

import gubg.ui;
import gubg.patterns.command;
import gubg.patterns.chainOfResponsibility;

interface IFocus: IChainOfResponsibility!(ICommand)
{
    void setIO(IInput input, IOutput output);
}

class FocusMgr: IChainOfResponsibility!(ICommand)
{
    this(IInput input, IOutput output)
        {
            _input = input;
            _output = output;
        }

    bool handle(inout ICommand command)
    {
	if (_handlers.length == 0)
	    return false;
	return _handlers[$-1].handle(command);
    }
    void successor(IChainOfResponsibility!(ICommand) handler){};

    bool push(IFocus handler)
    {
	auto catcher = new Catcher;
	if (_handlers.length > 0)
	{
	    catcher.successor = _handlers[$-1];
	    handler.successor = catcher;
	}
        // For now, just set the normal one. This should be replaced with a cached input variant.
        handler.setIO(_input, _output);
	_handlers ~= [handler];
	return true;
    }
    bool pop()
    {
	if (_handlers.length == 0)
	    return false;
	_handlers.length = _handlers.length-1;
	return true;
    }

private:
    class Catcher: IChainOfResponsibility!(ICommand)
    {
	bool handle4COR(inout ICommand command)
	{
	    // Reset the input cache
	    return false;
	}
	mixin ChainOfResponsibility!(ICommand);
    }

    IInput _input;
    IOutput _output;
    IFocus _handlers[];
}
