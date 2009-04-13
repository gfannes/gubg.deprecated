module rinle.view.view;

import rinle.view.nodeInfo;
import rinle.model.interfaces;

import gubg.ui;

import tango.text.convert.Format;

class View
{
    this (INode base)
        {
            mBase = base;
            mCurrent = base;
            mMgr = new Manager;
        }

    void show(Output output)
        {
	    auto ft = new FormatTree(Tag.create(mCurrent, Color.white, false));
	    addTo(ft, mCurrent);
            auto collector = new OutputCollector(output);
            collector(ft);
        }

private:

    void addTo(inout FormatTree ft, INode node)
	{
	    auto saveFT = ft;
	    // Add this node to the format tree
	    node.addTo(ft);

	    // Recursively descent, if indicated by the corresponding node info
	    if (mMgr.get(node).recurse)
		for (uint i = 0; i < node.nrComponents; ++i)
		    addTo(ft, node.replaceComponent(ReplaceMode.Get, i, null));
	    ft = saveFT;
	}

    INode mBase;
    INode mCurrent;
    Manager mMgr;
}
