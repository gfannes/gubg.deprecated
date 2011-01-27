module vix.model.Tab;

public import gubg.FSTree;

import std.path;

class Tab
{
    this()
    {
        creator_ = new Creator;
        currentFolder_ = Folder.createRecursive("/home/gfannes", creator_);
        currentFolder_.expand(creator_, ExpandStrat.Shallow);
    }

    string getPath(){return currentFolder_.path;}
    void moveToRoot()
    {
        if (currentFolder_.parent)
            currentFolder_ = currentFolder_.parent;
        currentFolder_.expand(creator_, ExpandStrat.Shallow);
    }
    FSTree[] getChilds()
    {
        FSTree[] childs;
        //Append only the non-hidden files and folders
        foreach (child; currentFolder_.childs)
            if (child.name[0] != '.')
                childs ~= child;
        return childs;
    }
    void setFolder(Folder folder)
    {
        currentFolder_ = folder;
        currentFolder_.expand(creator_, ExpandStrat.Shallow);
    }

    private:
        Creator creator_;
        Folder currentFolder_;
        //Filter filter_;
        //Selection selection_;
        //Current current_;
}

class Creator: ICreator
{
    Folder createFolder(string path)
    {
        return new Folder(path);
    }
    gubg.FSTree.File createFile(string path)
    {
        return new gubg.FSTree.File(path);
    }
}
