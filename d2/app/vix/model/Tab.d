module vix.model.Tab;

public import gubg.FSTree;

import std.path;
import std.regex;
import std.array;
import std.algorithm;

import std.stdio;

class Tab
{
    this()
    {
        creator_ = new Creator;
        setFolder(Folder.createRecursive("/home/gfannes", creator_));
    }

    string getPath(){return folder_.path;}
    void moveToRoot()
    {
        if (folder_.parent)
            setFolder(folder_.parent);
    }
    FSTree[] getChilds() {return childs_;}
    void setFolder(Folder folder)
    {
        folder_ = folder;
        folder_.expand(creator_, ExpandStrat.Shallow);
        filter_ = "";
        focus_ = "";
        updateChilds_();
    }

    string getFilter() const {return filter_;}
    void setFilter(string filter)
    {
        filter_ = filter;
        updateChilds_();
    }
    void appendToFilter(char c)
    {
        filter_ ~= c;
        updateChilds_();
    }
    string getFocus() const {return focus_;}
    void setFocus(string focus)
    {
        focus_ = focus;
        updateFocus_;
    }
    const(string[]) getSelection() const {return selection_;}
    void addToSelection(string selected){selection_ ~= selected;}

    private:
    void updateChilds_()
    {
        childs_.length = 0;
        //Append only the non-hidden files and folders which match the filter_
        auto re = regex(filter_.empty ? "." : filter_);
        foreach (child; folder_.childs)
            if (child.name[0] != '.')
            {
                auto m = match(child.name, re);
                if (!m.empty)
                    childs_ ~= child;
            }
        //Sort the childs using localCmp as criterion
        bool localCmp(FSTree lhs, FSTree rhs)
        {
            //First Folders, then Files
            if (cast(Folder)lhs && cast(gubg.FSTree.File)rhs)
                return true;
            if (cast(gubg.FSTree.File)lhs && cast(Folder)rhs)
                return false;
            //If the type results in a tie, sort alphabetically
            return lhs.name < rhs.name;
        }
        sort!(localCmp)(childs_);
        updateFocus_;
    }
    void updateFocus_()
    {
        if (childs_.empty)
            focus_ = "";
        else if (!canFind!("a.name == b")(childs_, focus_))
            focus_ = childs_[0].name;
    }

    Creator creator_;
    Folder folder_;
    FSTree[] childs_;
    string focus_;
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
