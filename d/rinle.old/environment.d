module rinle.environment;

import tango.core.Thread;

import rinle.interfaces;
import rinle.dir;
import rinle.sink;
import rinle.ui;
import rinle.registers;
import gubg.puts;

import tango.text.convert.Integer;

class Environment
{
    this(char[] pwd)
	{
	    mForwardDistance = 1;
	    mBackwardDistance = 2;

            Dir here;
            root = Dir.root(here, pwd);
            base = current = here;
	    mark = null;

	    mUI = new UI();
	    mUI.message("Welcome to Rinle", 0.3);
 	    mUI.log("Code away ... ");

	    mRegisters = new Registers;
	}

    ~this()
	{
	    mUI.message("Closing up", 0.3);
	    if (mUI)
		delete mUI;
	}

    char[] askString(char[] prefix){return mUI.askString(prefix);}

    bool getCache(inout INode res){return mRegisters.getCache(res);}
    bool getCache(inout INode[] res){return mRegisters.getCache(res);}
    void setCache(INode input){mRegisters.setCache(input);}
    void setCache(INode[] input){mRegisters.setCache(input);}
    void resetCache(){mRegisters.resetCache();}
    void pushStack(INode[] input){mRegisters.pushStack(input);}
    bool getStackTop(inout INode[] res){return mRegisters.getStackTop(res);}

    UI ui(){return mUI;}

    INode root;
    INode base;
    INode current;
    INode mark;
    int mForwardDistance;
    int mBackwardDistance;

    bool getRegion(inout INode s, inout INode e)
	{
	    if (mark is null || mark.parent is null || mark.parent !is current.parent)
		return false;
	    if (mark.childIX <= current.childIX)
	    {
		s = mark;
		e = current;
	    } else
	    {
		s = current;
		e = mark;
	    }
	    return true;
	}

    void goUp()
	{
	    auto sibling = current.navigate(Direction.Up);
	    if (sibling !is null)
		current = sibling;
	    refresh();
	}
    void goUpPage()
	{
	    for (int i = 0; i < 10; ++i)
	    {
		auto sibling = current.navigate(Direction.Up);
		if (sibling !is null)
		    current = sibling;
	    }
	    refresh();
	    mUI.input.clearKeyBuffer;
	}
    void goDown()
	{
	    auto sibling = current.navigate(Direction.Down);
	    if (sibling !is null)
		current = sibling;
	    refresh();
	}
    void goDownPage()
	{
	    for (int i = 0; i < 10; ++i)
	    {
		auto sibling = current.navigate(Direction.Down);
		if (sibling !is null)
		    current = sibling;
	    }
	    refresh();
	    mUI.input.clearKeyBuffer;
	}
    void goIn()
	{
	    auto firstChild = current.navigate(Direction.In);
	    if (firstChild !is null)
	    {
		current = firstChild;
		mark = null;
	    }
	    refresh();
	}
    void goOut()
	{
	    auto parent = current.navigate(Direction.Out);
	    if (parent !is null)
	    {
		if (current == base)
		    base = parent;
		current = parent;
		mark = null;
	    }
	    refresh();
	}

    void refresh()
	{
	    puts("\nStarting refresh, current = {}", current);
	    // Forward expand
	    current.eachRecursive(delegate void(INode n, uint dd)
			       {
				   puts("n = {} {} {}", cast(void*)n, dd <= mForwardDistance, n);
				   if (dd <= mForwardDistance)
				       n.expand();
			       });
	    puts("forward expand finished");
	    // Backward expand
	    bool passedBase = false;
	    int depth = 0;
	    current.toRoot(delegate bool(INode n)
			   {
				   if (depth <= mBackwardDistance && !passedBase)
				   {
				       n.eachRecursive(delegate void(INode nn, uint dd)
						       {
							   if (dd <= mBackwardDistance - depth)
							       nn.expand();
						       });
				   }
				   ++depth;
				   if (n == base)
				       passedBase = true;
				   return true;
			   });
	    puts("backward expand finished");
	    // Deselect everything and disable the show
	    base.eachRecursive(delegate void(INode n, uint depth)
			       {
				   n.lineIX = 0;
				   n.select = false;
				   n.show = false;
			       });
	    puts("Deselect finished");
	    // Show backward
	    passedBase = false;
	    depth = 0;
	    current.toRoot(delegate bool(INode n)
			   {
				   if (depth <= mBackwardDistance && !passedBase)
				   {
				       n.eachRecursive(delegate void(INode nn, uint dd)
						       {
							   if (dd <= mBackwardDistance - depth)
							       nn.show = nn.show | true;
						       });
				   }
				   n.show = n.show | true;
				   ++depth;
				   if (n == base)
				       passedBase = true;
				   return true;
			   });
	    puts("Show backward finished");
	    // Show forward
	    current.eachRecursive(delegate void(INode n, uint dd)
			       {
				   n.select = true;
				   n.show = (dd <= mForwardDistance);
			       });
	    puts("Show forward finished");

 	    scope sink = new Sink;
 	    base.render(sink);
	    puts("Rendering finished");
	    mUI.display(sink, current);
	    puts("End of refresh");
	}

private:
    UI mUI;
    Registers mRegisters;
}
