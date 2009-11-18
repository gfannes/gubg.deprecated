module gubg.Tree;

class Tree(T)
{
    class Node
    {
	this(T data, Node parent)
	    {
		mData = data;
		mParent = parent;
	    }

	T data(){return mData;}
	Node parent(){return mParent;}

	Node addChild(T data)
	    {
		auto node = new Node(data, this);
		mChilds ~= [node];
		return node;
	    }

	Node getChild(int ix)
	    {
		return mChilds[ix];
	    }

	int depth()
	    {
		int d;
		Node node = this;
		for (d = 0; node; node = node.parent())
		    ++d;
		return d;
	    }

	// Number if children
	int nrChilds(){return mChilds.length;}

	// Total number of nodes
	int nrNodes()
	    {
		int nr = 0;
		void counter(T data)
		{
		    ++nr;
		}
		eachNode(&counter);
		return nr;
	    }

	// Iterate over children
	void eachChild(void delegate(T data) callback)
	    {
		foreach(node; mChilds)
		{
		    callback(node.mData);
		}
	    }
	void eachChild(void delegate(Node node) callback)
	    {
		foreach(node; mChilds)
		{
		    callback(node);
		}
	    }

	// Iterate recursively (depth-first by default)
	void eachNode(void delegate(T data) callback, bool depthFirst = true)
	    {
		if (!depthFirst)
		    callback(mData);
		foreach(node; mChilds)
		{
		    node.eachNode(callback, depthFirst);
		}
		if (depthFirst)
		    callback(mData);
	    }
	void eachNode(void delegate(T[] datas) callback, bool depthFirst = true, T[] datas = [])
	    {
		datas ~= [mData];
		if (!depthFirst)
		    callback(datas);
		foreach(node; mChilds)
		{
		    node.eachNode(callback, depthFirst, datas);
		}
		if (depthFirst)
		    callback(datas);
		datas.length = datas.length-1;
	    }
	void eachNode(void delegate(Node node) callback, bool depthFirst = true)
	    {
		if (!depthFirst)
		    callback(this);
		foreach(node; mChilds)
		{
		    node.eachNode(callback, depthFirst);
		}
		if (depthFirst)
		    callback(this);
	    }

	// Collect information recursively (depth-first by default)
	void collectNodes(ref T[] datas, bool depthFirst = true)
	    {
		datas.length = nrNodes();
		datas.length = 0;
		void appendDatas(T data)
		{
		    datas ~= [data];
		}
		eachNode(&appendDatas, depthFirst);
	    }
	void collectNodes(ref Node[] nodes, bool depthFirst = true)
	    {
		nodes.length = nrNodes();
		nodes.length = 0;
		void appendNodes(Node node)
		{
		    nodes ~= [node];
		}
		eachNode(&appendNodes, depthFirst);
	    }

	// Iterate to root
	void toRoot(void delegate(T data) callback)
	    {
		callback(mData);
		if (parent())
		    parent().toRoot(callback);
	    }
	void toRoot(void delegate(Node node) callback)
	    {
		callback(this);
		if (parent())
		    parent().toRoot(callback);
	    }

	// Collect information to root
	void collectToRoot(ref T[] datas)
	    {
		datas.length = depth();
		datas.length = 0;
		void appendDatas(T data)
		{
		    datas ~= [data];
		}
		toRoot(&appendDatas);
	    }
	void collectToRoot(ref Node[] nodes)
	    {
		nodes.length = depth();
		nodes.length = 0;
		void appendNodes(Node node)
		{
		    nodes ~= [node];
		}
		toRoot(&appendNodes);
	    }

    private:
	Node[] mChilds;
	T mData;
	Node mParent;
    };

    this(){mRoot = null;}
    this(T data){setRoot(data);}

    Node root(){return mRoot;}
    Node setRoot(T data)
	{
	    // The root node has parent null
	    mRoot = new Node(data, null);
	    return mRoot;
	}

    bool empty(){return (root() is null);}
    int nrNodes()
	{
	    if (root())
		return root().nrNodes;
	    return 0;
	}

private:
    Node mRoot;
};

version(UnitTest)
{
    import gubg.Puts;

    void main()
    {
	auto tree = new Tree!(float);
	tree.setRoot(12.3);
	tree.root().addChild(45.6);
	tree.root().addChild(78.9);
	tree.root().addChild(01.2);
	puts("nrNodes = {}", tree.nrNodes());

	void printer(float data)
	{
	    puts("Data = {}", data);
	}
	tree.root().eachNode(&printer, false);

	auto node = tree.root().getChild(2);
	puts("Child 2 = {}", node.data());
	node.toRoot(&printer);

	float[] elements;
	tree.root().collectNodes(elements);
	foreach(el; elements)
	    puts("element = {}", el);

	node.collectToRoot(elements);
	foreach(el; elements)
	    puts("toRoot element = {}", el);
    }
}
