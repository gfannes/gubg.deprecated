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
template TMarkup(Tag, Content, Node, Context = void*)
{
    this ()
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

    void collect(Dest)(ref Dest dest, Stack!(MetaTag) stack = null, Context context = null)
	{
	    if (stack is null)
	    {
                stack = new Stack!(MetaTag);
                static if (is(Context == void*))
                {
                    beforeCollect();
                } else
                {
                    if (context is null)
                        context = new Context;
                    beforeCollect(context);
                }
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
                static if (is(Context == void*))
                    showBefore(dest, stack);
                else
                    showBefore(dest, stack, context);
		foreach(ix, el; mElements)
		{
		    meta.index = ix;
		    meta.first = (ix == 0);
		    meta.last = (ix == mElements.length-1);
                    static if (is(Context == void*))
                        collectElement(el, dest, stack);
                    else
                        collectElement(el, dest, stack, context);
		}
                static if (is(Context == void*))
                    showAfter(dest, stack);
                else
                    showAfter(dest, stack, context);
		stack.pop;
	    }
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

class XMLMarkup
{
    // Mixin the main markup functionality
    mixin TMarkup!(char[], char[], XMLMarkup);

    void beforeCollect()
        {
        }

    void showBefore(ref char[] dest, Stack!(MetaTag) stack)
	{
	    dest ~= indent(stack.depth-1) ~ "<" ~ mTag ~ (isLeaf ? ">" : ">\n");
	}
    void showAfter(ref char[] dest, Stack!(MetaTag) stack)
	{
	    dest ~= (isLeaf ? "" : indent(stack.depth-1)) ~ "</" ~ mTag ~ ">\n";
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
	auto m = new XMLMarkup;
	auto html = m.create("html");
	auto bdy = html.create("body");
	bdy.create("h1", "Welcome everybody");
	bdy.create("p", "This is a first paragraph");
	auto p = bdy.create("p", "This is the second paragraph. ");
	p.add("Second line. ");
	p.create("i", "eat this.");
	p.add(" And we continue.");

	char[] str;
	m.collect(str);
	puts("{}", str);
    }
}
