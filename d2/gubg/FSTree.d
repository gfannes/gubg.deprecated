module gubg.FSTree;

import gubg.Tree;
import gubg.Format;
import std.file;
import std.path;
import std.array;

import std.stdio;

class FSTree
{
    //Insert the tree functionality
    mixin Tree!(FSTree, Folder, File);

    //Interface
    abstract string toString() const;
    string path() const
    {
        return (parent ? join(parent.path, name) : name);
    }

    //This is the single data member, the name of the folder or file.
    //It is placed at the FSTree level, allowing access from an FSTree reference
    string name;
}

enum ExpandStrat {Shallow, Recursive};
class Folder: FSTree
{
    //Insert the node functionality
    mixin Node!(FSTree);

    this(string path, Folder folder = null)
    {
        name = basename(path);
        if (name.empty)
            name = path;
        parent = folder;
    }

    static Folder createRecursive(string path, ICreator creator)
    {
        //The top-level Folder that will be returned
        Folder res = creator.createFolder(path);
        //We will gradually step down until we reached to root of the file system
        string dir = dirname(path);
        Folder tmp = res;
        while (dir != path)
        {
            tmp.parent = creator.createFolder(dir);
            tmp = tmp.parent;
            auto t = dir;
            dir = dirname(dir);
            path = t;
        }
        return res;
    }

    void expand(ICreator creator, ExpandStrat et = ExpandStrat.Shallow)
    {
        if (!childs.empty)
            return;
        foreach (string subpath; dirEntries(path, SpanMode.shallow))
        {
            FSTree child;
            try
            {
                if (isdir(subpath))
                {
                    Folder folder = creator.createFolder(subpath);
		    if (folder)
		    {
			    folder.parent = this;
			    if (ExpandStrat.Recursive == et)
				    folder.expand(creator, et);
			    child = folder;
		    }
                }
                else if (isfile(subpath))
		{
                    child = creator.createFile(subpath);
		    if (child)
			    child.parent = this;
		}
            }
            catch (std.file.FileException){}

            if (child)
                childs ~= child;
        }
    }
    
    //FSTree interface
    string toString() const
    {
        return Format.immediate("Folder: %s containing %d childs.", name, childs.length);
    }
}

class File: FSTree
{
    //Insert the leaf functionality
    mixin Leaf!(FSTree);

    this(string lname, Folder folder = null)
    {
        name = basename(lname);
        parent = folder;
    }

    //FSTree interface
    string toString() const
    {
        return "File: " ~ name;
    }
}

interface ICreator
{
    Folder createFolder(string path);
    File createFile(string path);
}

FSTree createFSTreeFromPath(string path, ICreator creator)
{
    if (isdir(path))
    {
        auto folder = Folder.createRecursive(path, creator);
        if (folder)
            folder.expand(creator, ExpandStrat.Recursive);
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
	auto folder = Folder.createRecursive("/home/gfannes/gubg", creator);
        auto tree = createFSTreeFromPath("/home/gfannes/gubg", creator);
        foreach (File el; tree)
        {
            writeln(el.toString);
        }
    }
}
