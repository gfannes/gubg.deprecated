module rinle.node;

import tango.core.Array;

import rinle.interfaces;
import gubg.puts;

class Node: INode
{
    this(){}

    // Begin of INode implementation
    void render(Sink sink)
	{
//	    throw new Exception("Node.render");
	}
    void expand(){}

    void createChild(inout INode res, uint ix)
    {
	if (parent)
	    parent.createChild(res, ix);
    }
    void createCopy(inout INode res)
    {
	res = null;
	createCopyI(res);
	if (res !is null)
	    for (uint ix = 0; ix < nrChilds; ++ix)
	    {
		INode child;
		if (this[ix] !is null)
		    this[ix].createCopy(child);
		res.addChild(child);
	    }
    }
    void createCopyI(inout INode res)
    {
    }

    bool select(){return mSelect;}
    void select(bool b){mSelect = b;}
    uint lineIX(){return mLineIX;}
    void lineIX(uint lix){mLineIX = lix;}
    bool show(){return mShow;}
    void show(bool b){mShow = b;}
    bool save()
    {
	if (parent)
	    return parent.save();
	return false;
    }
    INode navigate(Direction dir)
    {
	INode res;
	switch (dir)
	{
	case Direction.Up:   res = prevSibling; break;
	case Direction.Down: res = nextSibling; break;
	case Direction.In:   res = this[0];     break;
	case Direction.Out:  res = parent;      break;
	default:
	    throw new Exception("Unknown direction");
	    break;
	}
	return res;
    }
    bool toRoot(bool delegate(INode node) yield)
	    {
		if (!yield(this))
		    return false;
		if (parent)
		    return parent.toRoot(yield);
		return true;
	    }
    uint nrChilds(){return mChilds.length;}
    bool setNrChilds(uint nr)
    {
	if (nr > mChilds.length)
	{
	    mChilds.length = nr;
	} else if (nr < mChilds.length)
	{
	    for (uint i = nr; i < mChilds.length; ++i)
		setChild(i, null);
	    mChilds.length = nr;
	}
	return nr == mChilds.length;
    }
    INode opIndex(int ix)
	{
	    if (ix >= 0 && ix < mChilds.length)
		return mChilds[ix];
	    return null;
	}
    INode nextSibling()
	{
	    INode sibling;
	    if (parent !is null)
		for (uint ix = childIX+1; ix < parent.nrChilds; ++ix)
		{
		    sibling = parent[ix];
		    if (sibling !is null)
			break;
		}
	    return sibling;
	}
    INode prevSibling()
	{
	    INode sibling;
	    if (parent !is null)
		if (childIX > 0)
		    for (uint ix = childIX-1; ix >= 0; --ix)
		    {
			sibling = parent[ix];
			if (sibling !is null)
			    break;
		    }
	    return sibling;
	}
    INode parent(){return mParent;}
    void parent(INode n){mParent = n;}
    void addChild(INode child){setChild(mChilds.length, child);}
    bool addChildBefore(uint ix, INode child){return insertChildBefore(ix, child);}
    bool addChildAfter(uint ix, INode child){return insertChildBefore(ix+1, child);}
    void eachRecursive(void delegate(INode node, uint depth) yield, uint depth = 0)
	{
	    yield(this, depth);
	    foreach(child; mChilds)
		if (child !is null)
		    child.eachRecursive(yield, depth + 1);
	}
    uint childIX(){return mChildIX;}
    void childIX(uint ix){mChildIX = ix;}
    INode remove()
    {
	if (parent is null)
	    return null;
	return parent.removeChild(childIX);
    }
    INode removeChild(uint ix)
    {
	INode n;
	if (ix < mChilds.length)
	{
	    n = mChilds[ix];
	    mChilds[ix] = null;
	    for (uint i = ix; i < mChilds.length - 1; ++i)
	    {
		mChilds[i] = mChilds[i+1];
		mChilds[i].childIX = i;
	    }
	    mChilds.length = mChilds.length - 1;
	}
	return n;
    }
    bool setChild(uint ix, INode child)
    {
	INode prev;
	if (!swapChild(prev, ix, child))
	    return false;
	if (prev !is null)
	    prev.parent(null);
	return true;
    }
    bool compress()
    {
	uint newLength = mChilds.length;
	for (uint i = mChilds.length-1; i >= 0; --i)
	    if (mChilds[i] is null)
	    {
		--newLength;
		for (uint j = i+1; j < newLength; ++j)
		    setChild(j-1, mChilds[j]);
	    }
	return true;
    }
    // End of INode implementation

    bool swapChild(out INode prev, uint ix, INode child)
	{
	    if (mChilds.length <= ix)
		mChilds.length = ix+1;
	    if (mChilds.length <= ix)
		return false;
	    prev = mChilds[ix];
	    mChilds[ix] = child;
	    if (child !is null)
	    {
		child.childIX = ix;
		child.parent = this;
	    }
	    return true;
	}
    bool insertChildBefore(uint ix, INode child)
	{
	    if (ix > mChilds.length)
		return false;

	    mChilds.length = mChilds.length+1;
	    INode prev, curr;
	    for (curr = child; ix < mChilds.length; ++ix)
	    {
		prev = mChilds[ix];
		
		curr.childIX = ix;
		curr.parent = this;
		mChilds[ix] = curr;
		
		curr = prev;
	    }
	    return true;
	}
    int eachChild(bool delegate(INode node) yield)
	{
	    int index;
	    foreach(ix, child; mChilds)
	    {
		if (!yield(child))
		{
		    index = ix;
		    break;
		}
	    }
	    return index;
	}
    int depth()
	{
	    int d;
	    INode node = this;
	    for (d = 0; node; node = node.parent)
		++d;
	    return d;
	}

    INode[] childs(){return mChilds;}

    void sortChilds(bool delegate(INode lhs, INode rhs) cmp)
	{
	    sort(mChilds, cmp);
	    foreach (ix, inout ch; mChilds)
		ch.childIX = ix;
	}

private:
    INode mParent;
    INode[] mChilds;
    uint mLineIX;
    bool mSelect;
    bool mShow;
    uint mChildIX;
}

version (UnitTest)
{
    import gubg.puts;
    void main()
    {
	auto root = new Node;
	Node n;
	root.addChild(n = new Node);
	root.addChild(new Node);
	root.addChild(new Node);
	root.addChild(new Node);
	n.addChild(new Node);
	n.addChild(new Node);

	bool del(Node n)
	{
	    puts("node {} (parent {})", cast(void*)n, cast(void*)n.parent);
	    n.eachChild(&del);
	    return true;
	}

	root.eachChild(&del);
    }
}