module rinle.model.d.d;

public import rinle.model.interfaces;
import rinle.model.d.parser;

import gubg.puts;
import gubg.file;
import gubg.parser;

abstract class DNode: INode
{
    bool create(inout ICommand command, IUI ui, bool delegate(INode node) setCurrent)
    {
	return false;
    }
    abstract void addTo(inout FormatTree ft, IFormatInfo delegate(INode node) formatInfo);    
}

class DModule: DNode, ICompositeNode
{
    this (char[] path, char[] name)
    {
        _path = path.dup;
        _name = name.dup;
        _expanded = false;
    }

    uint nrComponents(){return _declarations.length;}
    void setNrComponents(uint nr){_declarations.length = nr;}
    INode opIndex(uint ix){return _declarations[ix];}
    INode opIndexAssign(INode rhs, uint ix)
    {
        DDeclaration declaration = cast(DDeclaration)rhs;
        if (declaration is null)
            throw new Exception("Assignment of non-DDeclaration to DModule.");
        return (_declarations[ix] = declaration);
    }
    mixin TIndexComposite!(INodeMethods);

    void addTo(inout FormatTree ft, IFormatInfo delegate(INode node) formatInfo)
    {
	if (formatInfo(this).show)
	{
	    auto lft = ft.create(Tag.create(this, Color.white, true));
	    if (formatInfo(this).recurse)
		foreach (decl; _declarations)
		    decl.addTo(lft, formatInfo);
	}
    }
    void expand()
    {
        if (_expanded)
            return;

	char[] content;
	loadFile(content, _path ~ _name);

	auto parser = new DParser;
	parser.parse(this, content);

        _expanded = true;
    }
    bool create(inout INode node, uint ix, IUI ui)
    {
	return false;
    }
    mixin TUID;

private:
    char[] _path;
    char[] _name;

    DDeclaration[] _declarations;
    bool _expanded;
}

class DDeclaration: DNode
{
    abstract void addTo(inout FormatTree ft, IFormatInfo delegate(INode node) formatInfo);
    void expand()
    {
    }
    bool create(inout INode node, uint ix, IUI ui)
    {
	return false;
    }
    mixin TUID;

//     uint nrComponents(){return 0;}
//     void setNrComponents(uint nr){throw new ArrayBoundsException(__FILE__, __LINE__);}
//     IComponent!(INodeMethods) replaceComponent(ReplaceMode mode, uint ix, IComponent!(INodeMethods) newComponent)
//         {
//             throw new ArrayBoundsException(__FILE__, __LINE__);
//             return null;
//         }
//     bool isLeaf(){return true;}
//     int opApply(int delegate(IComponent!(INodeMethods) el) dg)
// 	{
// 	    return 0;
// 	}
//     mixin TParent!(INodeMethods);
}

class DIdentifier: DNode, ILeafNode
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
    bool create(inout INode node, uint ix, IUI ui)
    {
	return false;
    }
    mixin TUID;

private:
    char[] _identifier;
}

class DScope: DNode, ICompositeNode
{
    uint nrComponents(){return _declarations.length;}
    void setNrComponents(uint nr){_declarations.length = nr;}
    INode opIndex(uint ix){return _declarations[ix];}
    INode opIndexAssign(INode rhs, uint ix)
    {
        DDeclaration declaration = cast(DDeclaration)rhs;
        if (declaration is null)
            throw new Exception("Assignment of non-DDeclaration to DScope.");
        return (_declarations[ix] = declaration);
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
		foreach (decl; _declarations)
		    decl.addTo(lft, formatInfo);
	    lft.add("}");
	    lft.newline;
	}
    }
    void expand()
    {
    }
    bool create(inout INode node, uint ix, IUI ui)
    {
	return false;
    }
    mixin TUID;

private:
    DDeclaration[] _declarations;
}

class DModuleDeclaration: DDeclaration, ILeafNode
{
    this (char[] name)
    {
	_name = name.dup;
    }
    mixin TLeaf!(INodeMethods);

    void addTo(inout FormatTree ft, IFormatInfo delegate(INode node) formatInfo)
    {
	if (formatInfo(this).show)
	{
	    auto lft = ft.create(Tag.create(this, Color.red, false), "module ");
	    lft.create(Tag.create(this, Color.white, false), _name ~ ";");
	    lft.newline;
	}
    }
private:
    char[] _name;
}

class DImportDeclaration: DDeclaration, ILeafNode
{
    this (char[] name)
    {
	_name = name.dup;
    }
    mixin TLeaf!(INodeMethods);

    void addTo(inout FormatTree ft, IFormatInfo delegate(INode node) formatInfo)
    {
	if (formatInfo(this).show)
	{
	    auto lft = ft.create(Tag.create(this, Color.red, false), "import ");
	    lft.create(Tag.create(this, Color.white, false), _name ~ ";");
	    lft.newline;
	}
    }
private:
    char[] _name;
}

class DMixinDeclaration: DDeclaration, ILeafNode
{
    this (char[] name)
    {
	_name = name.dup;
    }
    mixin TLeaf!(INodeMethods);

    void addTo(inout FormatTree ft, IFormatInfo delegate(INode node) formatInfo)
    {
	if (formatInfo(this).show)
	{
	    auto lft = ft.create(Tag.create(this, Color.red, false), "mixin ");
	    lft.create(Tag.create(this, Color.white, false), _name ~ ";");
	    lft.newline;
	}
    }
private:
    char[] _name;
}

class DClassDeclaration: DDeclaration, ICompositeNode
{
    void setName(DIdentifier name)
    {
        replaceComponent(ReplaceMode.Set, 0, name);
    }
    void setBaseClasses(DBaseClasses baseClasses)
    {
        replaceComponent(ReplaceMode.Set, 1, baseClasses);
    }
    void setBody(DScope bdy)
    {
        replaceComponent(ReplaceMode.Set, 2, bdy);
    }

    void addTo(inout FormatTree ft, IFormatInfo delegate(INode node) formatInfo)
    {
	if (formatInfo(this).show)
	{
	    auto lft = ft.create(Tag.create(this, Color.red, false), "class ");
	    _name.addTo(lft, formatInfo);
	    _baseClasses.addTo(lft, formatInfo);
	    lft.newline;
	    _body.addTo(lft, formatInfo);
	}
    }

    INode opIndex(uint ix)
    {
        switch (ix)
        {
        case 0:
            return _name;
            break;
        case 1:
            return _baseClasses;
            break;
        case 2:
            return _body;
            break;
        default:
            throw new ArrayBoundsException(__FILE__, __LINE__);
            break;
        }
    }
    INode opIndexAssign(INode rhs, uint ix)
    {
        switch (ix)
        {
        case 0:
            return (_name = cast(DIdentifier)rhs);
            break;
        case 1:
            return (_baseClasses = cast(DBaseClasses)rhs);
            break;
        case 2:
            return (_body = cast(DScope)rhs);
            break;
        default:
            throw new ArrayBoundsException(__FILE__, __LINE__);
            return null;
            break;
        }
    }
    uint nrComponents(){return 3;}
    void setNrComponents(uint nr)
    {
        if (nr != 3)
            throw new ArrayBoundsException(__FILE__, __LINE__);
    }
    mixin TIndexComposite!(INodeMethods);

private:
    DIdentifier _name;
    DBaseClasses _baseClasses;
    DScope _body;
}

class DBaseClasses: DNode, ICompositeNode
{
    uint nrComponents(){return _baseClasses.length;}
    void setNrComponents(uint nr){_baseClasses.length = nr;}
    INode opIndex(uint ix){return _baseClasses[ix];}
    INode opIndexAssign(INode rhs, uint ix)
    {
	DIdentifier baseClass = cast(DIdentifier)rhs;
        if (baseClass is null)
            throw new Exception("Assignment of non-DIdentifier to DBaseClasses.");
        return (_baseClasses[ix] = baseClass);
    }
    mixin TIndexComposite!(INodeMethods);

    void addTo(inout FormatTree ft, IFormatInfo delegate(INode node) formatInfo)
    {
	if (formatInfo(this).show)
	{
	    auto lft = ft.create(Tag.create(cast(INode)this, Color.red, false), (_baseClasses.length > 0 ? ": " : " "));
	    if (formatInfo(this).recurse)
		foreach (ix, baseClass; _baseClasses)
                {
                    if (ix > 0)
                        lft.add(", ");
		    baseClass.addTo(lft, formatInfo);
                }
	}
    }
    void expand()
    {
    }
    bool create(inout INode node, uint ix, IUI ui)
    {
	return false;
    }
    mixin TUID;

private:
    DIdentifier[] _baseClasses;
}

version (UnitTest)
{
    void main()
    {
    }
}
