module rinle.model.interfaces;

public import gubg.patterns.composite;
import gubg.markup;
public import gubg.ui;

// All methods we require to be present for a Rinle node
interface NodeMethods
{
    void addTo(inout FormatTree ft);
}
// Some convenient aliases for working with composites
alias IComponent!(NodeMethods) INode;
alias IComposite!(NodeMethods) ICompositeNode;
alias ILeaf!(NodeMethods) ILeafNode;

// Tag information used for a FormatTree
struct Tag
{
    INode node;
    Color color;
    bool indent = true;
    bool spaceBefore = false;
    static Tag create(INode n = null, Color c = Color.white, bool i = true, bool s = false)
	{
	    Tag tag;
	    tag.node = n;
	    tag.color = c;
	    tag.indent = i;
	    tag.spaceBefore = s;
	    return tag;
	}
}

class Context
{
    this (){reset;}

    void reset()
        {
            rowIX = colIX = 0;
            newline = false;
        }

    int rowIX;
    int colIX;
    bool newline;
}

class FormatTree
{
    // Mixin the markup functionality
    mixin TMarkup!(Tag, char[], FormatTree, Context);

    void newline(){add("\n");}

private:
    void beforeCollect(Context ctx)
	{
            ctx.reset;
	}
    void showBefore(Dest: Output)(ref Dest dest, Stack!(MetaTag) stack, Context ctx)
	{
	    if (ctx.rowIX < dest.height)
	    {
		if (ctx.newline)
		{
		    mIndent = indent(stack);
		} else
		{
		    mIndent.length = ctx.colIX;
		    foreach (inout c; mIndent)
			c = ' ';
		}
	    }
	}
    void showAfter(Dest: Output)(ref Dest dest, Stack!(MetaTag) stack, Context ctx)
	{
	}
    void show(Dest: Output)(char[] content, ref Dest dest, Stack!(MetaTag) stack, Context ctx)
	{
	    auto tag = stack.top.tag;
	    ColorPair cp = {tag.color, (tag.color != Color.black ? Color.black : Color.red)};
	    if (ctx.rowIX < dest.height)
	    {
		if (ctx.newline)
		{
		    ctx.newline = false;
		    dest[++ctx.rowIX] = mIndent;
		    ctx.colIX = mIndent.length;
		}
		if (content == "\n")
		    ctx.newline = true;
		else
		{
		    if (tag.spaceBefore)
		    {
			dest.print(" ", ctx.rowIX, ctx.colIX);
			++ctx.colIX;
		    }
		    dest.print(content, ctx.rowIX, ctx.colIX, cp);
		    ctx.colIX += content.length;
		}
	    }
	}
    char[] indent(Stack!(MetaTag) stack)
	{
	    char[] ind;

	    uint nr = 0;
	    stack.each((MetaTag mt, uint d)
		       {
			   if (d > 0 && mt.tag.indent)
			       nr += 3;
		       });
	    ind.length = nr;
	    foreach (inout ch; ind)
		ch = ' ';
	    
	    return ind;
	}
    char[] mIndent;
}

version (UnitTest)
{
    import gubg.ncurses;
    void main()
    {
	auto ft = new FormatTree(Tag.create);
	ft.add("Line 1");
	ft.add("Line 1.1");
	ft.newline;
	ft.add("Line 2");
	ft.add("Line 2.1");
	auto sb = ft.create(Tag.create(null, Color.black, false, true));
	sb.add("Koekjes");
	sb.newline;

	auto l1 = ft.create(Tag.create(null, Color.green, true));
	l1.add("olala");
	l1.newline;

	auto l2 = l1.create(Tag.create(null, Color.cyan, true));
	l2.add("l2l2l2");

	// Output to ncurses
	{	
	    scope nc = new NCurses;
	    ft.collect(nc);
	    nc.refresh;
	    nc.getKey;
	}
	
	// Output to StdOutput
	scope output = new StdOutput;
	ft.collect(output);
	output.refresh;
    }
}
