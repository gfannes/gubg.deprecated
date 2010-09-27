module Collection;

public import gubg.Tree;
import std.path;

class DCollection
{
    this (string path)
    {
        creator_ = new DCreator;
        internalTree_ = createTreeFromPath(path, creator_);
        //This is hardcoded for now
        addExternalTree_("/home/gfannes/gubg/d2/gubg");
    }

    int opApply(int delegate(ref File) dg)
    {
        //We first iterate over the internal tree
        foreach (File file; internalTree_)
            if (dg(file)) break;
        //Next, we iterate over the external trees
        foreach (externalTree; externalTrees_)
        {
            foreach (File file; externalTree)
                if (dg(file)) break;
        }
        return 0;
    }

    private:
    void addExternalTree_(string path)
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

    class DCreator: ICreator
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
    DCreator creator_;
    Tree internalTree_;
    Tree[] externalTrees_;
}
