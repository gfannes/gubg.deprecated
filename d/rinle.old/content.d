module rinle.content;

import tango.io.File: TFile = File;

public import rinle.interfaces;
import rinle.node;
import rinle.fsnode;

abstract class Content: Node, IContent
{
    this()
    {
	mLoaded = false;
    }

    void expand()
    {
	if (!mLoaded)
	{
	    auto p = cast(FSNode)parent;
	    auto content = TFile(p.path).read;
	    try
	    {
		mLoaded = load(content);
	    }
	    catch
	    {
	    }
	}
    }

    bool save()
    {
	auto sink = new Sink;
	render(sink);
	char[] str;
	sink.showNormal();
	sink.collect(str);
	TFile((cast(FSNode)(parent)).path).write(cast(void[])(str));
	return true;
    }

private:
    bool mLoaded;
}
