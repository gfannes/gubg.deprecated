module rinle.view.view;

import rinle.view.nodeInfo;
import rinle.model.interfaces;
import rinle.formatTree;

class View
{
    this (INode base)
        {
            mBase = base;
            mCurrent = base;
            mMgr = new Manager;
        }

    FormatTree formatTree()
	{
	    auto ft = new FormatTree(Tag.create);
	    addTo(ft, mCurrent);
	    return ft;
	}

private:
    void addTo(inout FormatTree ft, INode node)
	{
	    // Add this node to the format tree
	    node.addTo(ft);

	    // Recursively descent, if indicated by the corresponding node info
	    if (mMgr.get(node).recurse)
		for (uint i = 0; i < node.nrComponents; ++i)
		    add2FormatTree(ft, node[i]);
	}

    INode mBase;
    INode mCurrent;
    Manager mMgr;
}
