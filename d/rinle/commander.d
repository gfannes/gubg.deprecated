module rinle.commander;

import gubg.puts;
import gubg.iofiber;

import rinle.ui;
import rinle.globals;
import rinle.interfaces;
import rinle.node;

alias void delegate() CommandOperation;

class Commander
{
    this(UI ui)
	{
	    mUI = ui;
	}

    bool waitForCommand()
	{
	    int key = mUI.input.getKey;
	    commandAbrev ~= [key];
	    Command *command;
	    command = (commandAbrev in mCommandMap);
	    if (command !is null)
	    {
		mCommand = *command;
		reset();
		return true;
	    }
	    return false;
	}

    bool executeCommand()
	{
	    mCommand.execute();
	    mCommand = null;
	    return true;
	}

    void addCommand(char[] abrev, char[] description, CommandOperation operation, bool printDescription = true)
	{
	    int[] abrevi;
	    abrevi.length = abrev.length;
	    foreach (ix, ch; abrev)
		abrevi[ix] = cast(int)ch;
	    mCommandMap[abrevi] = new Command(description, operation, printDescription);
	}

    void reset(){commandAbrev = [];}

    int[] commandAbrev;
    char[] commandString()
	{
	    char[] str;
	    str.length = commandAbrev.length;
	    foreach (ix, a; commandAbrev)
		str[ix] = cast(char)a;
	    return str;
	}
    char[] description()
	{
	    if (mCommand)
		return mCommand.description;
	    return "No command currently active";
	}

    class Command
    {
	this (char[] description, CommandOperation operation, bool printDescription)
	    {
		mDescription = description;
		mOperation = operation;
		mPrintDescription = printDescription;
	    }
	void execute()
	    {
		mOperation();
		if (mPrintDescription)
		    env.ui.log(mDescription);
	    }
	char[] description(){return mDescription;}
    private:
	char[] mDescription;
	CommandOperation mOperation;
	bool mPrintDescription;
    }


private:
    UI mUI;
    Command[int[]] mCommandMap;
    Command mCommand;
}

class NavigationCommander: Commander
{
    this(CommandOperation quit)
    {
	env = env;
	env.refresh();
	super(env.ui);
	addCommand("q", "Quitting ...", quit);
	addCommand("s", "", &env.goDown);
	addCommand("w", "", &env.goUp);
	addCommand("S", "", &env.goDownPage);
	addCommand("W", "", &env.goUpPage);
	addCommand("d", "Going in", &env.goIn);
	addCommand("a", "Going out", &env.goOut);
	addCommand("i", "Insert at end", &insertAtEnd);
	addCommand("o", "Insert after", &insertAfter);
	addCommand("u", "Insert before", &insertBefore);
	addCommand("r", "Replace", &replace);
	addCommand("e", "Strip", &strip);
	addCommand("z", "Undo", &undo);
	addCommand(" ", "Mark", &mark, false);
	addCommand("x", "push", &push);
	addCommand("X", "Delete", &remove);
	addCommand("c", "Copy", &copy);
	addCommand("v", "Pop before", &popBefore);
	addCommand("n", "Pop after", &popAfter);
	addCommand("b", "Pop after", &popAtEnd);
	addCommand("f", "Cycle up", &cycleUp);
	addCommand("g", "Cycle down", &cycleDown);
	addCommand("p", "Save", &save);
    }

    void insertAtEnd()
    {
	auto f = new IOFiber!(INode, uint)(&env.current.createChild, env.current.nrChilds);
	while (f.call)
	{
	    if (f.output !is null)
	    {
		env.current.addChild(f.output);
		env.current = f.output;
		break;
	    }
	}
	do
	{
	    env.refresh();
	} while (f.call);
    }

    void insertAfter()
    {
	INode p = env.current.parent;
	if (p !is null)
	{
	    auto f = new IOFiber!(INode, uint)(&p.createChild, env.current.childIX+1);
	    while (f.call)
	    {
		if (f.output !is null)
		{
		    p.addChildAfter(env.current.childIX, f.output);
		    env.current = f.output;
		    break;
		}
	    }
	    do
	    {
		env.refresh();
	    } while (f.call);
	}
    }
    void insertBefore()
    {
	INode p = env.current.parent;
	if (p !is null)
	{
	    INode node;
	    p.createChild(node, env.current.childIX);
	    if (node !is null)
	    {
		p.addChildBefore(env.current.childIX, node);
		env.current = node;
		env.refresh();
	    }
	}
    }

    void replace()
    {
	INode p = env.current.parent;
	if (p !is null)
	{
	    INode node;
	    p.createChild(node, env.current.childIX);
	    if (node !is null)
	    {
		p.setChild(env.current.childIX, node);
		env.current = node;
		env.refresh();
	    }
	}
    }

    void remove()
    {
	INode[] ary;
	remove(ary);
	env.setCache(ary);
    }

    void save()
    {
	env.current.save;
    }

    void mark()
    {
	if (env.mark is env.current)
	{
	    env.ui.log("Unmarked");
	    env.mark = null;
	} else
	{
	    env.ui.log("Marked");
	    env.mark = env.current;
	}
    }

    void cycleUp()
    {
	env.ui.message("CycleUp is not yet implemented.", 1);
    }

    void cycleDown()
    {
	env.ui.message("CycleDown is not yet implemented.", 1);
    }

    void popBefore()
    {
	env.ui.message("popBefore is not yet implemented.", 1);
    }

    void popAtEnd()
    {
	env.ui.message("popAtEnd is not yet implemented.", 1);
    }

    void popAfter()
    {
	env.ui.message("popAfter is not yet implemented.", 1);
    }

    void copy()
    {
	INode s, e;
	INode[] ary;
	if (env.getRegion(s, e))
	{
	    // We push the region to the stack
	    INode parent = s.parent;
	    for (uint ix = s.childIX; ix <= e.childIX; ++ix)
	    {
		INode copy;
		parent[ix].createCopy(copy);
		ary ~= [copy];
	    }
	} else
	{
	    // We copy the top of the stack into the stack
	    if (env.getStackTop(ary))
		foreach (ix, el; ary)
		    el.createCopy(ary[ix]);
	}
	env.pushStack(ary);
    }

    void push()
    {
	INode[] ary;
	remove(ary);
	env.pushStack(ary);
    }
    
    void remove(inout INode[] res)
    {
	INode s, e, node;
	if (env.getRegion(s, e))
	{
	    env.current = e;
	    env.goDown;
	    if (e is env.current)
	    {
		env.current = s;
		env.goUp;
		if (s is env.current)
		    env.goOut;
	    }

	    res.length = 0;
	    INode parent = e.parent;
	    for (uint ix = e.childIX; ix >= s.childIX; --ix)
	    {
		node = parent[ix];
		if (node.remove !is null)
		{
		    node.parent = null;
		    res ~= [node];
		}
	    }
	    res.reverse;
	    env.mark = null;
	} else
	{
	    node = env.current;
	    env.goDown;
	    if (node is env.current)
		env.goOut;
	    if (node.remove is null)
		env.current = node;
	    else
	    {
		node.parent = null;
		res = [node];
	    }
	}
	env.refresh;
    }

    void undo()
    {
	env.ui.message("Undo is not yet implemented.", 1);
    }

    void strip()
    {
	env.ui.message("Strip is not yet implemented.", 1);
    }
}
