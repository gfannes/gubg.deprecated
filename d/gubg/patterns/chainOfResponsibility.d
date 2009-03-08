module gubg.patterns.chainOfResponsibility;

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
	if (mSuccessor is null)
	{
	    throw new Exception("ERROR::Cannot handle the request, successor is null.");
	    return false;
	}
	return mSuccessor.handle(request);
    }
    void successor(IChainOfResponsibility!(Request) succ)
    {
	mSuccessor = succ;
    }

private:
    IChainOfResponsibility!(Request) mSuccessor;
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