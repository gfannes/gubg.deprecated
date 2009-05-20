module rinle.model.interfaces;

public import gubg.patterns.composite;
import gubg.tagTree;
public import gubg.ui;
import gubg.puts;

interface IFormatInfo
{
    bool show();
    bool recurse();
}

interface IUI
{
    bool getString(inout char[] str, char[] msg);
    bool selectString(inout uint ix, char[] msg, char[][] options);
}

// All methods we require to be present for a Rinle node
interface INodeMethods
{
    void addTo(inout FormatTree ft, IFormatInfo delegate(IComponent!(INodeMethods) node) formatInfo);
    void expand();
    bool create(inout IComponent!(INodeMethods) node, uint ix, IUI ui);
}
// Some convenient aliases for working with composites
alias IComponent!(INodeMethods) INode;
alias IComposite!(INodeMethods) ICompositeNode;
alias ILeaf!(INodeMethods) ILeafNode;

// Tag information used for a FormatTree
struct Tag
{
    INode node;
    Color color;
    bool indent = true;
    bool spaceBefore = false;
    bool invert = false;
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

class FormatTree: ITagTree!(Tag, char[])
{
    mixin TTagTree!(Tag, char[]);

    void newline(){add("\n");}

    static bool indexOfParent(T)(T node, inout uint ix)
    {
	auto p = node.parent;
	if (p is null)
	    return false;
	for (ix = 0; ix < p.nrComponents; ++ix)
	    if (p.replaceComponent(ReplaceMode.Get, ix, null) == node)
		break;
	if (ix >= p.nrComponents)
	    return false;
	return true;
    }
}

class OutputCollector: Collector!(Tag, char[])
{
    this (IOutput output)
    {
	_output = output;
    }
    void beforeCollect()
    {
	_indentLevel = 0;
	_rowIX = _colIX = 0;
    }
    void beforeTag(Tag tag, Stack!(MetaTag) stack)
    {
	_colorPair = color(tag);
	if (tag.spaceBefore)
	{
	    _output.print(" ", _rowIX, _colIX);
	    ++_colIX;
	}
	_currentIndent = indent(stack);
    }
    void afterTag(Tag tag, Stack!(MetaTag) stack)
    {
	MetaTag metaTag;
	if (stack.pop(metaTag) && stack.top !is null)
	{
	    auto t = stack.top.tag;
	    _colorPair = color(t);
	    _currentIndent = indent(stack);
	    stack.push(metaTag);
	}
    }
    void show(char[] content, Stack!(MetaTag) stack)
    {
	if (_rowIX < _output.height)
	{
	    if (_newline)
	    {
		_newline = false;
		_output[++_rowIX] = _currentIndent;
		_colIX = _currentIndent.length;
	    }
	    if (content == "\n")
		_newline = true;
	    else
	    {
		_output.print(content, _rowIX, _colIX, _colorPair);
		_colIX += content.length;
	    }
	}
    }
private:
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
    ColorPair color(Tag tag)
    {
	ColorPair cp = {tag.color, (tag.color != Color.black ? Color.black : Color.red)};
        if (tag.invert)
        {
            auto tmp = cp.foreground;
            cp.foreground = cp.background;
            cp.background = tmp;
        }
	return cp;
    }

    IOutput _output;
    ColorPair _colorPair;
    uint _indentLevel;
    uint _rowIX;
    uint _colIX;
    char[] _currentIndent;
    bool _newline;
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
	    scope collector = new OutputCollector(nc);
	    collector(ft);
	    nc.refresh;
	    nc.getKey;
	}
	
	// Output to StdOutput
	{
	    scope output = new StdOutput;
 	    auto collector = new OutputCollector(output);
 	    collector(ft);
 	    output.refresh;
	}
    }
}
