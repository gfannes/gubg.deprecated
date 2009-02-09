module rinle.registers;

import gubg.stack;
import rinle.interfaces;

// The following registers are available:
//  * The cache: the last one selected
//  * The stack: elements pushed and popped onto this stack
//  * The array: elements accessible via an index (0 .. 9)

alias INode[] Element;

class Registers
{
    this ()
	{
	    mStack = new Stack!(Element);
	}

    bool getCache(inout INode res)
	{
	    if (mCache.length == 1)
	    {
		res = mCache[0];
		return true;
	    }
	    return false;
	}
    bool getCache(inout INode[] res)
	{
	    if (mCache.length > 0)
	    {
		res[] = mCache[];
		return true;
	    }
	    return false;
	}
    void setCache(INode input)
	{
	    resetCache();
	    mCache = [input];
	}
    void setCache(INode[] input)
	{
	    resetCache();
	    mCache = input;
	}
    void resetCache(){mCache.length = 0;}

    void pushStack(INode[] input)
	{
	    mStack.push(input);
	}
    bool getStackTop(inout INode[] res)
	{
	    if (mStack.top is null)
		return false;
	    res = *mStack.top;
	    return true;
	}
private:
    Element mCache;
    Stack!(Element) mStack;
    Element[10] mArray;
}

version (Test)
{
    void main()
    {
	auto reg = new Registers;
    }
}
