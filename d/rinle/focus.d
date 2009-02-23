module rinle.focus;

import gubg.ui;
import gubg.patterns.command;
import gubg.patterns.chainOfResponsibility;

interface IFocus: IChainOfResponsibility!(ICommand)
{
    void setIO(Input input, Output output);
}

class FocusMgr: IChainOfResponsibility!(ICommand)
{
    this(Input input, Output output)
        {
            mInput = input;
            mOutput = output;
        }

    bool handle(inout ICommand command)
    {
	if (mHandlers.length == 0)
	    return false;
	return mHandlers[$-1].handle(command);
    }
    void successor(IChainOfResponsibility!(ICommand) handler){};

    bool push(IFocus handler)
    {
	auto catcher = new Catcher;
	if (mHandlers.length > 0)
	{
	    catcher.successor = mHandlers[$-1];
	    handler.successor = catcher;
	}
        // For now, just set the normal one. This should be replaced with a cached input variant.
        handler.setIO(mInput, mOutput);
	mHandlers ~= [handler];
	return true;
    }
    bool pop()
    {
	if (mHandlers.length == 0)
	    return false;
	mHandlers.length = mHandlers.length-1;
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

    Input mInput;
    Output mOutput;
    IFocus mHandlers[];
}
