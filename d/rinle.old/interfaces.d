module rinle.interfaces;

public import rinle.sink;
public import rinle.ui;

enum Direction
{
    Up,
	Down,
	In,
	Out
}

interface INode
{
    void render(Sink sink);
    void expand();

    void createChild(inout INode, uint ix);
    void createCopy(inout INode); // A deep copy
    static INode create()();	// An empty node
    static INode create(Info)(Info info); // Fill in the childs using info

    bool save();
    INode navigate(Direction dir);
    INode parent();
    void parent(INode n);
    bool toRoot(bool delegate(INode node) yield);
    uint nrChilds();
    bool setNrChilds(uint nr);
    INode opIndex(int ix);
    INode nextSibling();
    INode prevSibling();
    void eachRecursive(void delegate(INode node, uint depth) yield, uint depth = 0);
    void addChild(INode child);
    bool addChildBefore(uint ix, INode child);
    bool addChildAfter(uint ix, INode child);
    INode remove();
    INode removeChild(uint ix);
    bool setChild(uint ix, INode child);
    bool compress();		// Removes the null childs, returns true if allowed, else false

    bool select();
    void select(bool b);
    bool show();
    void show(bool b);
    uint lineIX();
    void lineIX(uint lix);
    uint childIX();
    void childIX(uint ix);
}

interface IContent
{
    char[] description();
    bool load(void[] content);
    bool save(ref void[] content);
}
