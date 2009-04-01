module rinle.sink;

import gubg.markup;
import rinle.line;
import rinle.interfaces;

import gubg.puts;

struct Tag
{
    INode node;
    short color;
    bool indent = true;
    bool spaceBefore = false;
}

class Sink
{
    void showAsTree()
	{
	    mShowAsTree = true;
	}
    void showNormal()
	{
	    mShowAsTree = false;
	}

    mixin TMarkup!(Tag, char[], Sink);

    void newline()
	{
	    add("\n");
	}

private:

    void beforeCollect()
	{
	    sNewline = true;
	}

    // Output to char[]
    void showBefore(ref char[] dest, Stack!(MetaTag) stack)
	{
	}
    void showAfter(ref char[] dest, Stack!(MetaTag) stack)
	{
	}
    void show(char[] content, ref char[] dest, Stack!(MetaTag) stack)
	{
	    Tag tag = stack.top.tag;
	    if (sNewline)
	    {
		dest ~= indent(stack);
		sNewline = false;
	    } else if (tag.spaceBefore)
		dest ~= ' ';
	    dest ~= content;
	    sNewline = (content == "\n");
	}

    // Output to Line[]
    void showBefore(ref Line[] dest, Stack!(MetaTag) stack)
	{
	    if (stack.top.tag.node !is null)
		stack.top.tag.node.lineIX = dest.length;
	}
    void showAfter(ref Line[] dest, Stack!(MetaTag) stack)
	{
	}
    void show(char[] content, ref Line[] dest, Stack!(MetaTag) stack)
	{
	    Tag tag = stack.top.tag;
	    if (sNewline)
	    {
		dest ~= [new Line];
		dest[$-1].add(indent(stack), 0);
		sNewline = false;
	    } else if (tag.spaceBefore)
		dest[$-1].add(" ", tag.color);
	    if (content == "\n")
		sNewline = true;
	    else
		dest[$-1].add(content, tag.color);
	}

    char[] indent(Stack!(MetaTag) stack)
	{
	    char[] ind;
	    if (mShowAsTree)
	    {
		ind.length = (stack.depth-1)*3;
		uint depth = 0;
		stack.each((MetaTag mt, uint d)
			   {
			       if (mt.tag.indent)
			       {			       
				   switch (d)
				   {
				   case 0:
				       break;
				   case 1:
				       ++depth;
				       if (mt.meta.last)
					   ind[$-depth*3 .. $-(depth-1)*3] = "`--";
				       else
					   ind[$-depth*3 .. $-(depth-1)*3] = "|--";
				       break;
				   default:
				       ++depth;
				       if (mt.meta.last)
					   ind[$-depth*3 .. $-(depth-1)*3] = "   ";
				       else
					   ind[$-depth*3 .. $-(depth-1)*3] = "|  ";
				       break;
				   }
			       }
			   });
		ind = ind[$-depth*3 .. $];
	    } else
	    {
		uint nr = 0;
		stack.each((MetaTag mt, uint d)
			   {
			       if (d > 0 && mt.tag.indent)
				   nr += 3;
			   });
		ind.length = nr;
		foreach (inout ch; ind)
		    ch = ' ';
	    }
	    return ind;
	}

    static bool sNewline;
    static bool mShowAsTree;
}
