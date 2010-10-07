module gubg.FSTree;

import gubg.Tree;
import std.file;
import std.path;
import std.format;
import std.range;

class FSTree: Tree!(string, string)
{
    // * Stringification
    abstract string toString() const;
    abstract string path() const;
}

class File: Leaf!(string, string)
{
    this(string path, Folder folder = null)
    {
        data = path;
        super(folder);
    }

    string toString() const
    {
        return "File: " ~ data;
    }
    string path() const
    {
        return join(parent.path, data);
    }
}

class Folder: Node!(string, string)
{
    this(string path, Folder folder = null)
    {
        data = path;
        super(folder);
    }
    
    string toString() const
    {
        auto writer = appender!(string);
        formattedWrite(writer, "Folder: %s containing %d childs.", data, childs.length);
        return writer.data;
    }
    string path() const
    {
        return (parent ? join(parent.path, data) : data);
    }
}

interface ICreator
{
    Folder createFolder(string path);
    File createFile(string path);
}

FSTree createTreeFromPath(string path, ICreator creator)
{
    if (isdir(path))
    {
        auto folder = creator.createFolder(path);
        if (folder)
        {
            foreach (string subpath; dirEntries(path, SpanMode.shallow))
            {
                FSTree tree = createTreeFromPath(subpath, creator);
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

version (FSTree)
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
