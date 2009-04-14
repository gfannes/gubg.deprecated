module gubg.tagTree;

import gubg.puts;

import gubg.patterns.composite;

public import gubg.stack;

abstract class Collector(Tag, Content)
{
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

    void opCall(ITagTree!(Tag, Content) tagTree)
        {
            collect(tagTree);
        }

    void beforeCollect(){}
    void afterCollect(){}
    void beforeTag(Tag tag, Stack!(MetaTag) stack){}
    void afterTag(Tag tag, Stack!(MetaTag) stack){}
    abstract void show(Content content, Stack!(MetaTag) stack);

private:    
    void collect(ITagTree!(Tag, Content) tagTree, Stack!(MetaTag) stack = null)
        {
            bool rootCall = false;
            if (stack is null)
            {
                rootCall = true;
                stack = new Stack!(MetaTag);
                beforeCollect();
            }

            MetaTag metaTag;
            metaTag.tag = tagTree.tag;
            metaTag.meta.length = tagTree.nrComponents;
            Meta* meta = &(stack.push(metaTag).meta);

            beforeTag(tagTree.tag, stack);

            for (uint ix = 0; ix < tagTree.nrComponents; ++ix)
            {
                meta.index = ix;
                meta.first = (ix == 0);
                meta.last = (ix == tagTree.nrComponents-1);

                auto el = tagTree.replaceComponent(ReplaceMode.Get, ix, null);
                if (el.isLeaf)
                {
                    auto leaf = cast(ContentLeaf!(Tag, Content))el;
                    show(leaf.content, stack);
                } else
                {
                    auto composite = cast(ITagTree!(Tag, Content))el;
                    collect(composite, stack);
                }
            }

            afterTag(tagTree.tag, stack);
            stack.pop;

            if (rootCall)
                afterCollect();
        }
}

// Component methods of a ITagTree
interface ITagTreeMethods(Tag, Content)
{
}

// This is the composite
interface ITagTree(Tag, Content): IComposite!(ITagTreeMethods!(Tag, Content))
{
    Tag tag();
    void setTag(Tag tag);
    void add(Content content);
    void add(ITagTree tt);
}
class TagTree(Tag, Content): ITagTree!(Tag, Content)
{
    mixin TTagTree!(Tag, Content);
}
template TTagTree(Tag, Content)
{
    alias IComponent!(ITagTreeMethods!(Tag, Content)) Component;
    alias ITagTree!(Tag, Content) ITT;
    this(Tag tag)
    {
        mTag = tag;
    }
    this (Tag tag, Content content)
    {
        this(tag);
        add(content);
    }

    void add(Content content)
    {
        replaceComponent(ReplaceMode.Create, nrComponents, new ContentLeaf!(Tag, Content)(content));
    }
    void add(ITT tt)
    {
        replaceComponent(ReplaceMode.Create, nrComponents, tt);
    }

    typeof(this) create(Tag tag)
    {
        auto child = new typeof(this)(tag);
        add(child);
        return child;
    }
    typeof(this) create(Tag tag, Content content)
    {
        auto child = new typeof(this)(tag, content);
        add(child);
        return child;
    }
    typeof(this) create(Tag tag, void delegate(ITT child) yield)
        {
            auto child = new typeof(this)(tag);
            yield(child);
            add(child);
            return child;
        }
    
    Tag tag()
        {
            return mTag;
        }
    void setTag(Tag tag)
        {
            mTag = tag;
        }

    // Create the specific composite methods
    uint nrComponents(){return mElements.length;}
    void setNrComponents(uint nr){mElements.length = nr;}
    Component opIndex(uint ix){return mElements[ix];}
    Component opIndexAssign(Component rhs, uint ix){return (mElements[ix] = rhs);}
    mixin TIndexComposite!(ITagTreeMethods!(Tag, Content));

private:
    Tag mTag;
    Component[] mElements;
}

class ContentLeaf(Tag, Content): ILeaf!(ITagTreeMethods!(Tag, Content))
{
    this (Content content)
    {
        mContent = content;
    }

    Content content()
    {
        return mContent;
    }

    // Leaf methods
    mixin TLeaf!(ITagTreeMethods!(Tag, Content));

private:
    Content mContent;
}

version (UnitTest)
{
    class XMLCollector: Collector!(char[], char[])
    {
        alias ITagTree!(char[], char[]) ITT;
        char[] str;
        uint indentLevel;
        void beforeCollect()
        {
            str.length = 0;
            indentLevel = 0;
        }
        void beforeTag(char[] tag, Stack!(MetaTag) stack)
        {
            str ~= indent ~ "<" ~ tag ~ ">\n";
            ++indentLevel;
        }
        void afterTag(char[] tag, Stack!(MetaTag) stack)
        {
            --indentLevel;
            str ~= indent ~ "</" ~ tag ~ ">\n";
        }
        void show(char[] content, Stack!(MetaTag) stack)
        {
            str ~= indent ~ content ~ "\n";
        }
    private:
        char[] indent()
        {
            char[] id;
            id.length = indentLevel*2;
            foreach (inout c; id)
                c = ' ';
            return id;
        }
    }

    void main()
    {
        auto html = new TagTree!(char[], char[])("html");
        auto bdy = html.create("body");
        bdy.create("h1", "Welcome everybody");
        bdy.create("p", "This is a first paragraph");
        auto p = bdy.create("p", "This is the second paragraph. ");
        p.add("Second line. ");
        p.create("i", "eat this.");
        p.add(" And we continue.");

        char[] str;
        auto collector = new XMLCollector;
        collector(html);
        puts("{}", collector.str);
    }
}

/+
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
+/
