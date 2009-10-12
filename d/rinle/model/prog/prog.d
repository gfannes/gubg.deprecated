module rinle.model.prog.prog;

public import rinle.model.interfaces;
import rinle.model.prog.parser;

import gubg.puts;
import gubg.file;
import gubg.parser;

abstract class ProgNode: INode
{
    abstract void addTo(inout FormatTree ft, IFormatInfo delegate(INode node) formatInfo);    
    void cut(){}
    void paste(){}
    mixin TCompact!(INodeMethods);
}

class ProgModule: ProgNode, ICompositeNode
{
    this (char[] path, char[] name)
    {
        _path = path.dup;
        _name = name.dup;
        _expanded = false;
    }

    uint nrComponents(){return _nodes.length;}
    void setNrComponents(uint nr){_nodes.length = nr;}
    INode opIndex(uint ix){return _nodes[ix];}
    INode opIndexAssign(INode rhs, uint ix)
    {
        ProgNode node = cast(ProgNode)rhs;
	if (rhs !is null && node is null)
            throw new Exception("Assignment of non-ProgNode to ProgModule.");
        return (_nodes[ix] = node);
    }
    mixin TIndexComposite!(INodeMethods);

    void addTo(inout FormatTree ft, IFormatInfo delegate(INode node) formatInfo)
    {
	if (formatInfo(this).show)
	{
	    auto lft = ft.create(Tag.create(this, Color.white, true));
	    if (formatInfo(this).recurse)
		foreach (decl; _nodes)
		    decl.addTo(lft, formatInfo);
	}
    }
    void expand()
    {
        if (_expanded)
            return;

	char[] content;
	loadFile(content, _path ~ _name);

	auto parser = new ProgParser;
	parser.parse(this, content);

        _expanded = true;
    }

    mixin TUID;

private:
    char[] _path;
    char[] _name;

    ProgNode[] _nodes;
    bool _expanded;
}

class ProgIdentifier: ProgNode, ILeafNode
{
    this (char[] identifier)
    {
	_identifier = identifier.dup;
    }
    mixin TLeaf!(INodeMethods);

    void addTo(inout FormatTree ft, IFormatInfo delegate(INode node) formatInfo)
    {
	if (formatInfo(this).show)
	    ft.create(Tag.create(this, Color.white, false), _identifier);
    }
    void expand()
    {
    }

    mixin TUID;

private:
    char[] _identifier;
}

class ProgSymbol: ProgNode, ILeafNode
{
    this (char[] symbol)
    {
	_symbol = symbol.dup;
    }
    mixin TLeaf!(INodeMethods);

    void addTo(inout FormatTree ft, IFormatInfo delegate(INode node) formatInfo)
    {
	if (formatInfo(this).show)
	    ft.create(Tag.create(this, Color.white, false), _symbol);
    }
    void expand()
    {
    }

    mixin TUID;

private:
    char[] _symbol;
}

class ProgScope: ProgNode, ICompositeNode
{
    uint nrComponents(){return _nodes.length;}
    void setNrComponents(uint nr){_nodes.length = nr;}
    INode opIndex(uint ix){return _nodes[ix];}
    INode opIndexAssign(INode rhs, uint ix)
    {
	ProgNode node = cast(ProgNode)rhs;
        if (node is null)
            throw new Exception("Assignment of non-ProgNode to ProgScope.");
        return (_nodes[ix] = node);
    }
    mixin TIndexComposite!(INodeMethods);

    void addTo(inout FormatTree ft, IFormatInfo delegate(INode node) formatInfo)
    {
	if (formatInfo(this).show)
	{
	    auto lft = ft.create(Tag.create(this, Color.white, true));
	    lft.add("{");
	    lft.newline;
	    if (formatInfo(this).recurse)
		foreach (decl; _nodes)
		    decl.addTo(lft, formatInfo);
	    lft.add("}");
	    lft.newline;
	}
    }
    void expand()
    {
    }

    mixin TUID;

private:
    ProgNode[] _nodes;
}

version (UnitTest)
{
    void main()
    {
    }
}
