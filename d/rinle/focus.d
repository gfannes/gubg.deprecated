module rinle.focus;

import gubg.ui;
import gubg.patterns.command;
import gubg.patterns.chainOfResponsibility;

class Focus: IChainOfResponsibility!(ICommand)
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

    bool push(IChainOfResponsibility!(ICommand) handler)
    {
	auto catcher = new Catcher;
	if (mHandlers.length > 0)
	{
	    catcher.successor = mHandlers[$-1];
	    handler.successor = catcher;
	}
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
    IChainOfResponsibility!(ICommand) mHandlers[];
}
