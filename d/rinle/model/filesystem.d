module rinle.model.filesystem;

import gubg.puts;

import rinle.model.interfaces;

import gubg.patterns.composite;
import gubg.file;

import tango.io.FilePath;
import tango.io.FileSystem;

abstract class FSNode: ICompositeNode
{
    this(char[] name)
	{
	    _name = name;
	}

    void addTo(inout FormatTree ft)
    {
	ft.add(_name);
	ft.newline;
    }

    char[] name(){return _name;}

//     void render(Sink sink)
//     {
//  	if (show)
//  	{
// 	    Tag tag;
// 	    tag.node = this;
// 	    tag.color = (select ? 1 : 0);
// 	    auto h = sink.create(tag, _name);
// 	    h.newline;
// 	    foreach (ix, ch; childs)
// 		ch.render(h);
// 	}
//     }

//     char[] name(){return _name;}

//     char[] path()
//     {
//         char[] path;
// 	FSNode f = this;
//         while (f !is null)
//         {
// 	    if (path.length == 0)
// 		path = f._name;
// 	    else
// 		path = f._name ~ "/" ~ path;
//             f = cast(FSNode)f.parent;
//         }
//         return path;
//     }

private:
    char[] _name;
}

class File: FSNode
{
    this (char[] name)
    {
        super(name);
    }

    mixin TLeaf!(NodeMethods);
}

class Dir: FSNode
{
    this()
    {
        super(FileSystem.getDirectory);
	mExpanded = false;
    }

    uint nrComponents(){return mFSNodes.length;}
    void setNrComponents(uint nr){mFSNodes.length = nr;}
    INode opIndex(uint ix){return mFSNodes[ix];}
    INode opIndexAssign(INode rhs, uint ix)
    {
        FSNode fsNode = cast(FSNode)rhs;
        if (fsNode is null)
            throw new Exception("Assignment of non-FSNode to Dir.");
        return (mFSNodes[ix] = fsNode);
    }
    mixin TIndexComposite!(NodeMethods);

    void addTo(inout FormatTree ft)
    {
	expand;
	ft.add(_name);
	ft.newline;
    }

    void expand()
    {
	if (mExpanded)
	    return;

        foreach (fileInfo; FilePath(_name))
        {
            auto newName = fileInfo.path ~ fileInfo.name;
            if (fileInfo.folder)
            {
                scope d = new ChangeDir(newName);
                mFSNodes ~= [new Dir];
            } else
                mFSNodes ~= [new File(newName)];
        }

	mExpanded = true;
    }

private:
    FSNode[] mFSNodes;
    bool mExpanded;
}

version (UnitTest)
{
    void main()
    {
        auto dir = new Dir;
        dir.expand();
    }
}
