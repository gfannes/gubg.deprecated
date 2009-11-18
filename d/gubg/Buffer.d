module gubg.Buffer;

import gubg.File;

class Buffer
{
    this(in char[] fileName)
	{
	    if (!loadFile(buffer, fileName))
		throw new Exception("Could not load file " ~ fileName);
	}

    Index index()
	{
	    return new Index;
	}

    class Index
    {
	this ()
	    {
		if (current !is null)
		    startIX = currentIX = current.currentIX;
		previous = current;
		current = this;
	    }
	~this ()
	    {
		if (previous !is null)
		    previous.currentIX = currentIX;
		current = previous;
	    }
    private:
	uint startIX;
	uint currentIX;
	Index previous;
    }

private:
    char[] buffer;
    Index current;
}

version (UnitTest)
{
    void main()
    {
	auto buffer = new Buffer("buffer.d");
    }
}
