module ulbu.builder;

import gubg.puts;

import ulbu.element;

class Builder
{
    this ()
	{
	    mRoot = new Root;
	}

    void build(char[] dirName, char[] fileName)
        {
            auto el = Element.createFrom(dirName, fileName);
	    mRoot.add(el);
	    mRoot.setLocation();
        }

    void print()
	{
	    mRoot.print();
	}

private:
    Root mRoot;
}

version (Test)
{
    void main()
    {
        auto builder = new Builder;

        builder.build("test", "test.ulbu");
	builder.print();
    }
}
