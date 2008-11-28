module ulbu.builder;

import tango.io.File;

import gubg.puts;

import ulbu.element;
import ulbu.primitive;

class Builder
{
    this ()
	{
	    mRoot = new Root;
	}

    void build(char[] dirName, char[] fileName)
        {
	    // Create the element for compilation
            auto el = Element.createFrom(dirName, fileName);
	    mRoot.add(el);
	    // Add the system elements
	    addSystemElements();
	    // Set the locations into the elements
	    mRoot.setLocation();
	    // Resolve the unknown bodies
	    Body[char[]] fn2Body;
	    mRoot.collectBodies(fn2Body);
	    mRoot.resolveBodies(fn2Body);
        }

    void compile(char[] fileName)
        {
            scope fo = new File(fileName);
            fo.write([]);
            bool write2File(Element element)
            {
                if (!element.isRoot)
                    fo.append(element.fullName ~ "\n");
                return true;
            }
            mRoot.depthFirst(&write2File);
        }

    void print()
	{
	    mRoot.print();
	}

private:
    void addSystemElements()
	{
	    mRoot.add(new Element(new Attributes([]), new Name("int"), new Integer));
	    mRoot.add(new Element(new Attributes([]), new Name("byte"), new Byte));
	}

    Root mRoot;
}

version (Test)
{
    void main()
    {
        auto builder = new Builder;

        builder.build("test", "test.ulbu");
	builder.print();
        builder.compile("test.s");
    }
}
