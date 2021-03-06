module rinle.model.Filesystem;

import gubg.Puts;

import rinle.model.Interfaces;
import rinle.model.d.D;
import rinle.model.prog.Prog;

import gubg.patterns.Composite;
import gubg.File;

import tango.io.FilePath;
import tango.io.FileSystem;
import tango.core.Array;

abstract class FSNode: ICompositeNode
{
    this(char[] path, char[] name)
	{
	    _path = path.dup;
	    _name = name.dup;
	}

    abstract void addTo(inout FormatTree ft, IFormatInfo delegate(INode node) formatInfo);
    abstract void expand();
    abstract void cut();
    abstract void paste();

    mixin TUID;
    mixin TCompact!(INodeMethods);

    char[] path(){return _path;}
    char[] name(){return _name;}

protected:
    char[] _path;
    char[] _name;
}

class File: FSNode
{
    this (char[] path, char[] name)
    {
        super(path, name);
    }


    uint nrComponents(){return ((_syntaxTree is null) ? 0 : 1);}
    void setNrComponents(uint nr){}
    INode opIndex(uint ix)
    {
        if (ix == 0)
            return _syntaxTree;
	return null;
    }
    INode opIndexAssign(INode rhs, uint ix)
    {
        if (ix == 0)
            return (_syntaxTree = rhs);
        return null;
    }
    mixin TIndexComposite!(INodeMethods);

    void addTo(inout FormatTree ft, IFormatInfo delegate(INode node) formatInfo)
    {
	if (formatInfo(this).show)
	{
	    auto lft = ft.create(Tag.create(this, Color.white, false));
	    lft.add(_name);
	    lft.newline;
	    if (_syntaxTree !is null && formatInfo(this).recurse)
		_syntaxTree.addTo(lft, formatInfo);
	}
    }    
    void expand()
    {
	if (_syntaxTree !is null)
            return;
        switch (extension)
        {
        case "d":
//	    replaceComponent(ReplaceMode.Set, 0, new DModule(path, name));
	    replaceComponent(ReplaceMode.Set, 0, new ProgModule(path, name));
            break;
        default:
            break;
        }
    }
    void cut()
    {
	// TODO::Remove this file
    }
    void paste()
    {
	// TODO::Put back this file
    }

private:
    char[] extension()
    {
        return FilePath(_name).ext;
    }

    INode _syntaxTree;
}

class Dir: FSNode
{
    this ()
    {
	auto dir = FileSystem.getDirectory;
	dir.length = dir.length-1;
	auto fp = FilePath(dir);
	super(fp.parent ~ "/", fp.file);
    }
    this (char[] path, char[] name)
    {
        super(path, name);
	_expanded = false;
    }

    uint nrComponents(){return _fsNodes.length;}
    void setNrComponents(uint nr){_fsNodes.length = nr;}
    INode opIndex(uint ix){return _fsNodes[ix];}
    INode opIndexAssign(INode rhs, uint ix)
    {
        FSNode fsNode = cast(FSNode)rhs;
        if (rhs !is null && fsNode is null)
            throw new Exception("Assignment of non-FSNode to Dir.");
        return (_fsNodes[ix] = fsNode);
    }
    mixin TIndexComposite!(INodeMethods);

    void addTo(inout FormatTree ft, IFormatInfo delegate(INode node) formatInfo)
    {
	if (formatInfo(this).show)
	{
	    auto lft = ft.create(Tag.create(this, Color.green, true));
	    lft.add(_name);
	    lft.newline;
	    if (formatInfo(this).recurse)
		foreach (fsNode; _fsNodes)
		    fsNode.addTo(lft, formatInfo);
	}
    }    
    void expand()
    {
	if (_expanded)
	    return;

        foreach (fileInfo; FilePath(_path ~ _name))
        {
            if (fileInfo.folder)
            {
                _fsNodes ~= [new Dir(fileInfo.path, fileInfo.name)];
            } else
                _fsNodes ~= [new File(fileInfo.path, fileInfo.name)];
        }
        sort(_fsNodes, delegate bool(FSNode lhs, FSNode rhs){return lhs.name < rhs.name;});
	foreach (fsNode; _fsNodes)
	    fsNode.parent(this);

	_expanded = true;
    }
    void cut()
    {
	// TODO::Remove this directory
    }
    void paste()
    {
	// TODO::Put back this directory
    }

private:

    FSNode[] _fsNodes;
    bool _expanded;
}

version (UnitTest)
{
    void main()
    {
        auto dir = new Dir;
        dir.expand();
    }
}