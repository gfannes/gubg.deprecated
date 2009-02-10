module gubg.patterns.chainOfResponsibility;

// Chain of responsibility pattern

interface IChainOfResponsibility(Request)
{
    bool handle(Request request);
}

class ChainOfResponsibility(Request): IChainOfResponsibility!(Request)
{
    bool handle(Request request)
    {
	if (mSuccessor is null)
	{
	    throw new Exception("ERROR::Cannot handle the request, successor is null.");
	    return false;
	}
	return mSuccessor.handle(request);
    }

private:
    ChainOfResponsibility mSuccessor;
}

version (Test)
{
    int main()
    {
	auto cor = new ChainOfResponsibility!(int);
	cor.handle(1);
	return 0;
    }
}
