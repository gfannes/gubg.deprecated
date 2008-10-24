module gubg.test;

import gubg.puts;
import gubg.markup;

class RinleMarkup
{
    void showAsTree()
	{
	    mShowAsTree = true;
	}
    void showNormal()
	{
	    mShowAsTree = false;
	}

    mixin Markup!(int, char[], RinleMarkup);

    void newline()
	{
	    add("\n");
	}

private:

    void beforeCollect()
	{
	    sNewline = true;
	}
    void showBefore(ref char[] dest, Stack!(MetaTag) stack)
	{
	}
    void showAfter(ref char[] dest, Stack!(MetaTag) stack)
	{
	}
    void show(char[] content, ref char[] dest, Stack!(MetaTag) stack)
	{
	    if (sNewline)
	    {
		dest ~= indent(stack);
		sNewline = false;
	    } else
		dest ~= ' ';
	    dest ~= content;
	    sNewline = (content == "\n");
	}
    char[] indent(Stack!(MetaTag) stack)
	{
	    char[] ind;
	    ind.length = (stack.depth-1)*3;
	    if (mShowAsTree)
	    {
		stack.each((MetaTag mt, uint depth)
			   {
			       switch (depth)
			       {
			       case 0:
				   break;
			       case 1:
				   if (mt.meta.last)
				       ind[$-depth*3 .. $-(depth-1)*3] = "`--";
				   else
				       ind[$-depth*3 .. $-(depth-1)*3] = "|--";
				   break;
			       default:
				   if (mt.meta.last)
				       ind[$-depth*3 .. $-(depth-1)*3] = "   ";
				   else
				       ind[$-depth*3 .. $-(depth-1)*3] = "|  ";
				   break;
			       }
			   });
	    } else
	    {
		foreach (inout ch; ind)
		    ch = ' ';
	    }
	    return ind;
	}

    static bool sNewline;
    static bool mShowAsTree;
}

void main()
{
    puts("Start");

    auto m = new RinleMarkup;

    auto i = m.create(0);
    i.add("if (");
    auto b = i.create(1);
    b.add("a");
    b.add("==");
    b.add("0");
    i.add(")");
    i.newline;
    i.add("{");
    i.newline;
    auto bdy = i.create(1);
    bdy.add("z");
    bdy.add("=");
    bdy.add("123");
    bdy.newline;
    bdy.add("a");
    bdy.add("+=");
    bdy.add("3");
    bdy.newline;
    i.add("}");
    i.newline;

    char[] str;
    m.showAsTree();
    m.collect(str);
    puts("{}", str);

    puts("End");
}
