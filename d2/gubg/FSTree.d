module gubg.FSTree;

import std.file;
import std.path;
import std.format;
import std.range;

class Tree
{
    //Abstract interface
    // * Iteration over all elements of a certain type
    abstract int opApply(int delegate(ref Tree) dg);
    abstract int opApply(int delegate(ref File) dg);
    abstract int opApply(int delegate(ref Folder) dg);
    // * Stringification
    abstract string toString() const;
    abstract string path() const;

    //Parent access
    Folder parent;
}

class File: Tree
{
    this(string path, Folder folder = null)
    {
        path_ = path;
        parent = folder;
    }

    //Abstract interface implementation from Tree
    int opApply(int delegate(ref Tree) dg)
    {
        Tree me = this;
        return dg(me);
    }
    int opApply(int delegate(ref File) dg)
    {
        return dg(this);
    }
    int opApply(int delegate(ref Folder) dg)
    {
        return 0;
    }
    string toString() const
    {
        return "File: " ~ path_;
    }
    string path() const
    {
        return join(parent.path, path_);
    }

    private:
    string path_;
}

class Folder: Tree
{
    Tree[] childs;

    this(string path, Folder folder = null)
    {
        path_ = path;
        parent = folder;
    }
    
    //Abstract interface implementation from Tree
    int opApply(int delegate(ref Tree) dg)
    {
        Tree me = this;
        auto res = dg(me);
        if (res) return res;
        foreach (child; childs)
        {
            res = child.opApply(dg);
            if (res) return res;
        }
        return 0;
    }
    int opApply(int delegate(ref File) dg)
    {
        foreach (child; childs)
        {
            auto res = child.opApply(dg);
            if (res) return res;
        }
        return 0;
    }
    int opApply(int delegate(ref Folder) dg)
    {
        auto res = dg(this);
        if (res) return res;
        foreach (child; childs)
        {
            Folder subfolder = cast(Folder)child;
            if (subfolder)
            {
                res = subfolder.opApply(dg);
                if (res) return res;
            }
        }
        return 0;
    }
    string toString() const
    {
        auto writer = appender!(string);
        formattedWrite(writer, "Folder: %s containing %d childs.", path_, childs.length);
        return writer.data;
    }
    string path() const
    {
        return (parent ? join(parent.path, path_) : path_);
    }

    private:
    string path_;
}

interface ICreator
{
    Folder createFolder(string path);
    File createFile(string path);
}

Tree createTreeFromPath(string path, ICreator creator)
{
    if (isdir(path))
    {
        auto folder = creator.createFolder(path);
        if (folder)
        {
            foreach (string subpath; dirEntries(path, SpanMode.shallow))
            {
                Tree tree = createTreeFromPath(subpath, creator);
                if (tree)
                {
                    tree.parent = folder;
                    folder.childs ~= tree;
                }
            }
        }
        return folder;
    } else if (isfile(path))
        return creator.createFile(path);

    throw new Exception("Path is neither a directory nor a file.");
    return null;
}

version (UnitTest)
{
    import std.stdio;
    import std.path;
    void main()
    {
        class Creator: ICreator
        {
            Folder createFolder(string path)
            {
                return new Folder(path);
            }
            File createFile(string path)
            {
                if ("d" == getExt(path))
                    return new File(path);
                return null;
            }
        }
        Creator creator = new Creator;
        auto tree = createTreeFromPath("/home/gfannes/gubg", creator);
        foreach (File el; tree)
        {
            writeln(el.toString);
        }
    }
}
