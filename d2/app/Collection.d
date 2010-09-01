module Collection;

public import gubg.Tree;
import std.path;

class DCollection
{
    this (string path)
    {
        auto creator = new DCreator;
        internalTree_ = createFromPath(path, creator);
        //This is hardcoded for now
        externalTrees_ ~= createFromPath("/home/gfannes/gubg/d2/gubg", creator);
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
    Tree internalTree_;
    Tree[] externalTrees_;
}
