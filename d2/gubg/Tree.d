module gubg.Tree;

import std.file;
import std.format;
import std.range;

interface ITree
{
    int opApply(int delegate(ref ITree) dg);
    int opApply(int delegate(ref File) dg);
    int opApply(int delegate(ref Folder) dg);
    string toString() const;
}

class File: ITree
{
    this(string path)
    {
        path_ = path;
    }
    int opApply(int delegate(ref ITree) dg)
    {
        ITree me = this;
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

    private:
    string path_;
}

class Folder: ITree
{
    this(string path)
    {
        path_ = path;
        foreach (DirEntry de; dirEntries(path_, SpanMode.shallow))
        {
            ITree tree;
            if (de.isdir())
            {
                tree = new Folder(de.name);
            } else if (de.isfile())
            {
                tree = new File(de.name);
            } else throw new Exception("Entry is neither dir nor file.");
            childs ~= tree;
        }
    }
    int opApply(int delegate(ref ITree) dg)
    {
        ITree me = this;
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

    ITree[] childs;

    private:
    string path_;
}

ITree createFromPath(string path)
{
    ITree res;
    if (isdir(path))
    {
        res = new Folder(path);
    } else if (isfile(path))
    {
        res = new File(path);
    }
    return res;
}

import std.stdio;
void main()
{
    auto tree = createFromPath("/home/gfannes/gubg");
    foreach (Folder el; tree)
    {
        writeln(el.toString);
    }
}
