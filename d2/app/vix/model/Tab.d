module vix.model.Tab;

public import gubg.FSTree;

import std.path;
import std.regex;
import std.array;

import std.stdio;

class Tab
{
    this()
    {
        creator_ = new Creator;
        folder_ = Folder.createRecursive("/home/gfannes", creator_);
        folder_.expand(creator_, ExpandStrat.Shallow);
    }

    string getPath(){return folder_.path;}
    void moveToRoot()
    {
        if (folder_.parent)
            folder_ = folder_.parent;
        folder_.expand(creator_, ExpandStrat.Shallow);
    }
    FSTree[] getChilds()
    {
        FSTree[] childs;
        //Append only the non-hidden files and folders which match the filter_
        auto re = regex(filter_.empty ? "." : filter_);
        foreach (child; folder_.childs)
            if (child.name[0] != '.')
            {
                auto m = match(child.name, re);
                if (!m.empty)
                    childs ~= child;
            }
        return childs;
    }
    void setFolder(Folder folder)
    {
        folder_ = folder;
        folder_.expand(creator_, ExpandStrat.Shallow);
    }

    string getFilter() const {return filter_;}
    void setFilter(string filter){filter_ = filter;}
    const(string[]) getSelection() const {return selection_;}
    void addToSelection(string selected){selection_ ~= selected;}

    private:
        Creator creator_;
        Folder folder_;
        string filter_;
        string[] selection_;
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
