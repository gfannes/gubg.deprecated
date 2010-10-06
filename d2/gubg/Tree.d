module gubg.Tree;

import std.exception;

class Tree(LeafData, NodeData)
{
    alias Leaf!(LeafData, NodeData) LeafT;
    alias Node!(LeafData, NodeData) NodeT;

    //Abstract interface
    // * Iteration over all elements of a certain type
    abstract int opApply(int delegate(ref Tree) dg);
    abstract int opApply(int delegate(ref LeafT) dg);
    abstract int opApply(int delegate(ref NodeT) dg);
    // * RTTI
    NodeT isNode(){return null;}
    LeafT isLeaf(){return null;}
    bool isRoot(){return parent is null;}
    // * Construction
    void remove()
    {
        if (parent)
        {
            scope(exit) parent = null;
            enforce(parent.removeChild(this));
        }
    }

    //Data
    union
    {
        //When LeafData == NodeData, we provide access to it via data
        static if (is(LeafData == NodeData))
        {
            LeafData data;
        } else
        {
            LeafData leafData_;
            NodeData nodeData_;
        }
    };
    Tree set(T)(T d)
        if (is(T == NodeData) && is(T == LeafData))
    {
        data = d;
        return this;
    }
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

    static if (!is(LeafData == NodeData))
    {
        alias leafData_ data;
    }
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
    bool removeChild(TreeT child)
    {
        foreach (ix, ref TreeT ch; childs)
            if (ch is child)
            {
                childs = childs[0 .. ix] ~ childs[ix+1 .. $];
                return true;
            }
        return false;
    }

    static if (!is(LeafData == NodeData))
    {
        alias nodeData_ data;
    }
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
        //Leaf and node data are different
        alias Tree!(int, string) TreeT;
        auto root = (new TreeT.NodeT).set("Root node");
        for (uint i = 0; i < 10; ++i)
        {
            if (i % 2)
                root.createLeaf(i);
            else with (root.createNode(Format.immediate("Subnode %d", i)))
            {
                createLeaf(10*i);
                auto l = createLeaf(10*i+1);
                l.remove;
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

        //Leaf and node data are the same, note that we can now access the data using a Tree reference
        alias Tree!(string, string) Tree2T;
        auto root2 = (new Tree2T.NodeT).set("Root2 node");
        foreach (Tree2T el; root2)
        {
            writefln("%s", el.data);
        }
    }
}
