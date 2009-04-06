module gubg.markup;

import gubg.puts;

public import gubg.stack;

// Template to be used for creating a tree of elements of type Content that have a markup Tag attached to them.
// Define the following functions
//  * void beforeCollect(Context context)
//     * Called before collect() starts collecting info from the tree
//  * void showBefore(ref Dest dest, Stack!(MetaTag) stack, Context context)
//     * Dest is the type specified in collect, typically char[]
//     * Add things before content is added to dest
//  * void showAfter(ref Dest dest, Stack!(MetaTag) stack, Context context)
//     * Add things after content is added to dest
//  * void show(Content content, ref Dest dest, Stack!(MetaTag) stack, Context context)
// Node should be set to the class where this template is mixed-in
// Constructors are provided by the template
// Context is optional for the above methods, in case you want to have a place to keep things during collect

class Collector(Node)
{
    alias Node.TagT Tag;

    struct Meta
    {
	uint index;
	uint length;
	bool first;
	bool last;
    }
    struct MetaTag
    {
	Meta meta;
	Tag tag;
    }

    void opCall(Dest, Node)(inout Dest dest, Node node)
	{
	}
    
    void collect(Dest)(ref Dest dest, Stack!(MetaTag) stack = null)
	{
	    if (stack is null)
	    {
                stack = new Stack!(MetaTag);
		beforeCollect();
	    }
	    if (isRoot)
		foreach(el; mElements)
		    collectElement(el, dest, stack);
	    else
	    {
		MetaTag metaTag;
		metaTag.tag = mTag;
		metaTag.meta.length = mElements.length;
		Meta* meta = &(stack.push(metaTag).meta);
		showBefore(dest, stack);
		foreach(ix, el; mElements)
		{
		    meta.index = ix;
		    meta.first = (ix == 0);
		    meta.last = (ix == mElements.length-1);
		    collectElement(el, dest, stack);
		}
		showAfter(dest, stack);
		stack.pop;
	    }
	}

    void collectElement(Dest)(Element el, ref Dest dest, Stack!(MetaTag) stack, Context context = null)
	{
            static if (is(Context == void*))
            {
                if (el.isChild)
                    el.u.child.collect(dest, stack);
                else
                    show(el.u.content, dest, stack);
            } else
            {
                if (el.isChild)
                    el.u.child.collect(dest, stack, context);
                else
                    show(el.u.content, dest, stack, context);
            }
	}
}

template TMarkupNode(Tag, Content, Node)
{
    alias Tag TagT;
    this()
	{
	    isRoot = true;
	}
    this (Tag tag)
        {
	    isRoot = false;
            mTag = tag;
        }
    this (Tag tag, Content content)
        {
            this(tag);
	    addContent(content);
        }

    void add(Content content)
	{
	    if (isRoot)
		throw new Exception("You cannot add content directly to the root");
	    addContent(content);
	}
    Node create(Tag tag)
        {
	    auto child = new Node(tag);
	    addChild(child);
	    return child;
        }
    Node create(Tag tag, Content content)
        {
	    auto child = new Node(tag, content);
	    addChild(child);
	    return child;
        }
    Node create(Tag tag, void delegate(Node child) yield)
        {
	    auto child = new Node(tag);
	    yield(child);
	    addChild(child);
	    return child;
        }

    bool isLeaf()
	{
	    bool leaf = true;
	    foreach(el; mElements)
		leaf = leaf && (!el.isChild);
	    return leaf;
	}

    Tag markupTag(){return mTag;}

private:
    void addChild(Node child)
	{
	    U u;
	    u.child = child;
	    Element el;
	    el.isChild = true;
	    el.u = u;
	    mElements ~= [el];
	}
    void addContent(Content content)
	{
	    U u;
	    u.content = content;
	    Element el;
	    el.isChild = false;
	    el.u = u;
	    mElements ~= [el];
	}
    
    bool isRoot;
    Tag mTag;
    union U
    {
	Content content;
	Node child;
    }
    struct Element
    {
	bool isChild;
	U u;
    }
    Element[] mElements;
}

class XMLNode
{
    // Mixin the main markup functionality
    mixin TMarkupNode!(char[], char[], XMLNode);
}

class XMLCollector: Collector!(XMLNode)
{

    void beforeCollect()
        {
        }

    void showBefore(ref char[] dest, XMLNode node, Stack!(MetaTag) stack)
	{
	    dest ~= indent(stack.depth-1) ~ "<" ~ node.tag ~ (node.isLeaf ? ">" : ">\n");
	}
    void showAfter(ref char[] dest, XMLNode node, Stack!(MetaTag) stack)
	{
	    dest ~= (node.isLeaf ? "" : indent(stack.depth-1)) ~ "</" ~ node.tag ~ ">\n";
	}
    void show(char[] content, ref char[] dest, Stack!(MetaTag) stack)
	{
	    dest ~= content;
	}
    
private:
    char[] indent(uint depth)
        {
            char[] ind;
            ind.length = depth*2;
            foreach (inout ch; ind)
                ch = ' ';
            return ind;
        }
}

version (UnitTest)
{

    void main()
    {
	auto m = new XMLNode;
	auto html = m.create("html");
	auto bdy = html.create("body");
	bdy.create("h1", "Welcome everybody");
	bdy.create("p", "This is a first paragraph");
	auto p = bdy.create("p", "This is the second paragraph. ");
	p.add("Second line. ");
	p.create("i", "eat this.");
	p.add(" And we continue.");

	char[] str;
	auto collector = new XMLCollector;
	collector(str, m);
	puts("{}", str);
    }
}
