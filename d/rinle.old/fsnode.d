module rinle.fsnode;

import rinle.interfaces;
import rinle.node;
import rinle.sink;
import gubg.puts;

class FSNode: Node
{
    this(char[] n)
	{
	    mName = n;
	}

    void render(Sink sink)
    {
 	if (show)
 	{
	    Tag tag;
	    tag.node = this;
	    tag.color = (select ? 1 : 0);
	    auto h = sink.create(tag, mName);
	    h.newline;
	    foreach (ix, ch; childs)
		ch.render(h);
	}
    }

    char[] name(){return mName;}

    char[] path()
    {
        char[] path;
	FSNode f = this;
        while (f !is null)
        {
	    if (path.length != 0)
		path = f.mName ~ "/" ~ path;
	    else
		path = f.mName;
            f = cast(FSNode)f.parent;
        }
        return path;
    }

private:
    char[] mName;
}
