module gubg.Tree;

import std.exception;

mixin template Tree(TreeType, NodeType, LeafType)
{
    alias TreeType TreeT;
    alias NodeType NodeT;
    alias LeafType LeafT;

    NodeT parent;

    //Provided functionality and interface
    // * Iteration over all elements of a certain type
    abstract int opApply(int delegate(ref TreeT) dg);
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
}
mixin template Leaf(TreeType)
{
    alias TreeType TreeT;
    alias TreeType.NodeT NodeT;
    alias TreeType.LeafT LeafT;

    int opApply(int delegate(ref TreeT) dg)
    {
        TreeT me = this;
        return dg(me);
    }
    int opApply(int delegate(ref LeafT) dg)
    {
        return dg(this);
    }
    int opApply(int delegate(ref NodeT) dg)
    {
        return 0;
    }

    LeafT isLeaf(){return this;}
}
mixin template Node(TreeType)
{
    alias TreeType TreeT;
    alias TreeType.NodeT NodeT;
    alias TreeType.LeafT LeafT;

    TreeT[] childs;

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
    int opApply(int delegate(ref NodeT) dg)
    {
        auto res = dg(this);
        if (res) return res;
        foreach (child; childs)
        {
            NodeT subfolder = cast(NodeT)child;
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
    NodeT add(TreeT tree)
    {
        childs ~= tree;
        return tree.parent = this;
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
}

version (Tree)
{
    import gubg.Format;
    import std.stdio;
    mixin template Setter(T)
    {
        T set(string d)
        {
            data = d;
            return this;
        }
    }
    class T
    {
        mixin Tree!(T, N, L);
        string data;
    }
    class N: T
    {
        mixin Node!(T);
        mixin Setter!(N);
    }
    class L: T
    {
        mixin Leaf!(T);
        mixin Setter!(L);
    }
    void main()
    {
        auto n = (new N).set("Root node");
        for (uint i = 0; i < 10; ++i)
            n.add((new L).set(Format.immediate("Leaf %d", i)));
        foreach (T t; n)
        {
            if (t.isRoot)
                writefln("Tree: %s", t.data);
            else
                writefln("Tree: %s.%s", t.parent.data, t.data);
        }
    }
}
