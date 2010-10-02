module Collection;

public import gubg.Tree;
import std.path;
import std.stdio;

class DCollection
{
    this (string path)
    {
        creator_ = new DCreator;
        internalTree_ = createTreeFromPath(path, creator_);
    }

    int opApply(FileT)(int delegate(ref FileT) dg)
    {
        //We first iterate over the internal tree
        foreach (gubg.Tree.File file; internalTree_)
        {
            FileT f = cast(FileT)file;
            if (f && dg(f)) break;
        }
        //Next, we iterate over the external trees
        foreach (externalTree; externalTrees_)
        {
            foreach (gubg.Tree.File file; externalTree)
            {
                FileT f = cast(FileT)file;
                if (f && dg(f)) break;
            }
        }
        return 0;
    }

    void addExternalTree(string path)
    {
        //Check that the path is not the same as the internal or any external path
        if (path == internalTree_.path)
            return;
        foreach (externalTree; externalTrees_)
            if (path == externalTree.path)
                return;

        //The path seems not to be present yet, so we add it
        externalTrees_ ~= createTreeFromPath(path, creator_);
    }

    void prune(string filepath)
    {
        writeln("Starting with pruning");
        foreach(ref DFile file; this)
        {
            if (filepath != file.path)
                file.isTagged = true;
        }
        foreach(ref DFile file; this)
        {
            if (file.isTagged)
                file.remove;
        }
        writeln("Pruning is done");
        assert(false);
    }

    private:
    class DFile: gubg.Tree.File
    {
        this(string path, Folder folder = null)
        {
            super(path, folder);
        }

        bool isTagged;
    }
    class DCreator: ICreator
    {
            Folder createFolder(string path)
            {
                return new Folder(path);
            }
            DFile createFile(string path)
            {
                if ("d" == getExt(path))
                    return new DFile(path);
                return null;
            }
    }
    DCreator creator_;
    Tree internalTree_;
    Tree[] externalTrees_;
}
