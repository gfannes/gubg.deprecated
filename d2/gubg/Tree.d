module gubg.Tree;

class Tree(LeafData, NodeData)
{
    alias Leaf!(LeafData, NodeData) LeafT;
    alias Node!(LeafData, NodeData) NodeT;

    //Abstract interface
    // * Iteration over all elements of a certain type
    abstract int opApply(int delegate(ref Tree) dg);
    abstract int opApply(int delegate(ref LeafT) dg);
    abstract int opApply(int delegate(ref NodeT) dg);
//    abstract int opApply(int delegate(ref LeafData) dg);
//    abstract int opApply(int delegate(ref NodeData) dg);
    // * RTTI
    NodeT isNode(){return null;}
    LeafT isLeaf(){return null;}
    bool isRoot(){return parent is null;}

    //Parent access
    NodeT parent;
}

class Leaf(LeafData, NodeData): Tree!(LeafData, NodeData)
{
    alias Tree!(LeafData, NodeData) TreeT;
    alias Node!(LeafData, NodeData) NodeT;

    this(NodeT p = null)
    {
        parent = p;
    }

    //Abstract interface implementation from Tree
    int opApply(int delegate(ref TreeT) dg)
    {
        TreeT me = this;
        return dg(me);
    }
    int opApply(int delegate(ref Leaf) dg)
    {
        return dg(this);
    }
    int opApply(int delegate(ref NodeT) dg)
    {
        return 0;
    }

    LeafT isLeaf(){return this;}

    LeafData data;
    Leaf set(LeafData d)
    {
        data = d;
        return this;
    }
}

class Node(LeafData, NodeData): Tree!(LeafData, NodeData)
{
    alias Tree!(LeafData, NodeData) TreeT;
    alias Leaf!(LeafData, NodeData) LeafT;

    this(NodeT p = null)
    {
        parent = p;
    }

    //Abstract interface implementation from Tree
    int opApply(int delegate(ref TreeT) dg)
    {
        TreeT me = this;
        auto res = dg(me);
        if (res) return res;
        foreach (child; childs)
        {
            res = child.opApply(dg);
            if (res) return res;
        }
        return 0;
    }
    int opApply(int delegate(ref LeafT) dg)
    {
        foreach (child; childs)
        {
            auto res = child.opApply(dg);
            if (res) return res;
        }
        return 0;
    }
    int opApply(int delegate(ref Node) dg)
    {
        auto res = dg(this);
        if (res) return res;
        foreach (child; childs)
        {
            Node subfolder = cast(Node)child;
            if (subfolder)
            {
                res = subfolder.opApply(dg);
                if (res) return res;
            }
        }
        return 0;
    }
    NodeT isNode(){return this;}

    //Construction support
    Node add(TreeT tree)
    {
        childs ~= tree;
        return this;
    }
    Node createNode(NodeData d)
    {
        auto n = (new Node(this)).set(d);
        add(n);
        return n;
    }
    LeafT createLeaf(LeafData d)
    {
        auto l = (new LeafT(this)).set(d);
        add(l);
        return l;
    }

    NodeData data;
    Node set(NodeData d)
    {
        data = d;
        return this;
    }
    TreeT[] childs;
}

version (Tree)
{
    import gubg.Format;
    import std.stdio;
    void main()
    {
        alias Tree!(int, string) TreeT;
        auto root = (new TreeT.NodeT).set("Root node");
        for (uint i = 0; i < 10; ++i)
        {
            if (i % 2)
                root.createLeaf(i);
            else
            {
                with (root.createNode(Format.immediate("Subnode %d", i)))
                {
                    createLeaf(10*i);
                    createLeaf(10*i+1);
                }
            }
        }
        foreach (TreeT el; root)
        {
            if (auto n = el.isNode)
            {
                writefln("%s: %s", (n.parent ? n.parent.data : "This is the root"), n.data);
            } else if (auto l = el.isLeaf)
            {
                writefln("%s: %d", l.parent.data, l.data);
            }
        }
    }
}
