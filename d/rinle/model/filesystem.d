module rinle.model.filesystem;

import gubg.puts;

import rinle.model.interfaces;
import gubg.patterns.composite;
import gubg.file;

import tango.io.FileSystem;
import tango.io.FileScan;

abstract class FSNode: ICompositeNode
{
    this(char[] name)
	{
	    mName = name;
	}

    

//     void render(Sink sink)
//     {
//  	if (show)
//  	{
// 	    Tag tag;
// 	    tag.node = this;
// 	    tag.color = (select ? 1 : 0);
// 	    auto h = sink.create(tag, mName);
// 	    h.newline;
// 	    foreach (ix, ch; childs)
// 		ch.render(h);
// 	}
//     }

//     char[] name(){return mName;}

//     char[] path()
//     {
//         char[] path;
// 	FSNode f = this;
//         while (f !is null)
//         {
// 	    if (path.length == 0)
// 		path = f.mName;
// 	    else
// 		path = f.mName ~ "/" ~ path;
//             f = cast(FSNode)f.parent;
//         }
//         return path;
//     }

private:
    char[] mName;
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
    }

    uint nrComponents(){return mFiles.length;}
    void setNrComponents(uint nr){mFiles.length = nr;}
    INode opIndex(uint ix){return mFiles[ix];}
    INode opIndexAssign(INode rhs, uint ix)
    {
        FSNode fsNode = cast(FSNode)rhs;
        if (fsNode is null)
            throw new Exception("Assignment of non-FSNode to Dir.");
        return (mFiles[ix] = fsNode);
    }
    mixin TIndexComposite!(NodeMethods);

    void expand()
    {
        auto scan = new FileScan;
        scan(".", true);
        foreach (file; scan.files)
            mFiles ~= [new File(file.name)];
        foreach (dir; scan.folders)
        {
            scope d = new ChangeDir(dir.name);
            mFiles ~= [new Dir];
        }
    }

private:
    FSNode[] mFiles;
}

version (UnitTest)
{
    void main()
    {
        auto dir = new Dir;
        dir.expand();
    }
}
