module rinle.file;

import tango.io.FilePath;

import rinle.interfaces;
import rinle.fsnode;
import rinle.node;
import rinle.sink;
import rinle.content;
import rinle.lines.lines;
import rinle.d.d;
import gubg.puts;

class File: FSNode
{
    this(char[] n)
	{
	    super(n);
	}

    void expand()
    {
	// We only load if necessary
	if (nrChilds == 0)
	{
	    auto ext = FilePath(name).ext;
	    switch (ext)
	    {
	    case "d":
		mContent = new DContent;
		break;
	    default:
		mContent = new LineContent;
		break;
	    }
	    addChild(mContent);
	}
    }

    INode navigate(Direction dir)
    {
	INode res = super.navigate(dir);
	if (cast(LineContent)res)
	    res = res[0];
	return res;
    }

    INode remove()
    {
	FilePath(path()).remove;
	return super.remove;
    }

    void createChild(inout INode res, uint ix){}

private:
    Content mContent;
}
