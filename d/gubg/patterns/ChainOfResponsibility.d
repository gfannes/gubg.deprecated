module gubg.patterns.ChainOfResponsibility;

// Chain of responsibility pattern

interface IChainOfResponsibility(Request)
{
    bool handle(inout Request request);
    void successor(IChainOfResponsibility!(Request) succ);
}

template ChainOfResponsibility(Request)
{
    bool handle(inout Request request)
    {
	if (handle4COR(request))
	    return true;
	if (_successor is null)
	{
	    throw new Exception("ERROR::Cannot handle the request, successor is null.");
	    return false;
	}
	return _successor.handle(request);
    }
    void successor(IChainOfResponsibility!(Request) succ)
    {
	_successor = succ;
    }

private:
    IChainOfResponsibility!(Request) _successor;
}

version (UnitTest)
{
    class Test
    {
	bool handle4COR(inout int request)
	    {
		return true;
	    }
	mixin ChainOfResponsibility!(int);
    }

    int main()
    {
	auto cor = new Test;
	int request = 1;
	cor.handle(request);
	return 0;
    }
}
