module vix.model.Tab;

public import gubg.FSTree;
import gubg.Format;

import std.path;

//There was instability in std.regex for D2.052 which is why we for now fall back to using regexp
version = UseRegExp;
version (UseRegex)
    import std.regex;
version (UseRegExp)
    import std.regexp;
import std.array;
import std.algorithm;
import std.process;
import std.file;

import std.stdio;

class Tab
{
    this(string folder, string contentPattern = "")
    {
        if (!contentPattern.empty)
        {
            contentPattern_ = contentPattern;
            reContentPattern_ = RegExp(contentPattern);
        }
        creator_ = new Creator(reContentPattern_);
        setFolder(folder);
    }

    string getContentPattern(){return contentPattern_;}
    void refresh()
    {
        folder_.expand(creator_, ExpandStrat.Shallow, true);
    }
    string getPath(){return folder_.path;}
    void moveToRoot()
    {
        if (folder_.parent)
            activate(folder_.parent);
    }
    FSTree[] getChilds(ref uint focusIX)
    {
        focusIX = focusIX_;
        return childs_;
    }
    void setFolder(string folder)
    {
        activate(Folder.createRecursive(folder, creator_));
    }
    void activate(FSTree tree)
    {
        auto folder = cast(Folder)tree;
        if (!(folder is null))
        {
            folder_ = folder;
            folder_.expand(creator_, ExpandStrat.Shallow);
            filter_ = "";
            focus_ = "";
            updateChilds_();
            return;
        }
        auto file = cast(gubg.FSTree.File)tree;
        if (!(file is null))
        {
            writefln("Executing file %s", file.path);
            system(Format.immediate("gvim --remote-tab-silent \"%s\"", file.path));
            version (Posix) system("wmctrl -a GVIM");
        }
    }
    void activateFocus()
    {
        if (childs_.empty)
            return;
        activate(childs_[focusIX_]);
    }
    enum Movement {Up, Down}
    void moveFocus(Movement movement)
    {
        switch (movement)
        {
            case Movement.Up:
                if (focusIX_ > 0)
                    --focusIX_;
                break;
            case Movement.Down:
                if (focusIX_ < childs_.length-1)
                    ++focusIX_;
                break;
        }
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
        //Append only the non-hidden files and folders which match the filter_ (case insensitive)
        auto reStr = (filter_.empty ? "." : filter_);
        version (UseRegex)
            auto re = regex(reStr, "i");
        version (UseRegExp)
            auto re = RegExp(reStr, "i");
        foreach (child; folder_.childs)
            if (child.name[0] != '.')
            {
                version (UseRegex)
                {
                    auto m = match(child.name, re);
                    if (!m.empty)
                        childs_ ~= child;
                }
                version (UseRegExp)
                {
                    if (re.test(child.name))
                        childs_ ~= child;
                }
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
        {
            focus_ = "";
            focusIX_ = 0;
            return;
        }
        foreach (uint ix, child; childs_)
        {
            if (focus_ == child.name)
            {
                focusIX_ = ix;
                return;
            }
        }
        focus_ = childs_[0].name;
        focusIX_ = 0;
    }

    Creator creator_;
    Folder folder_;
    FSTree[] childs_;
    string focus_ = "";
    uint focusIX_ = 0;
    string filter_;
    string[] selection_;
    string contentPattern_;
    RegExp reContentPattern_;
}

class Creator: ICreator
{
    this (RegExp contentPattern = null)
    {
        reContentPattern_ = contentPattern;
        reExtension_ = RegExp("\\.(d|c|h|cpp|txt|xml)$");
    }
    RegExp reContentPattern_;
    RegExp reExtension_;

    Folder createFolder(string path)
    {
        return new Folder(path);
    }
    gubg.FSTree.File createFile(string path)
    {
        if (!(reContentPattern_ is null))
        {
            if (!reExtension_.test(path))
                return null;
            writefln("Reading file: %s", path);
            auto content = readText!(string)(path);
            if (content.length > 100_000 || !reContentPattern_.test(content))
                return null;
        }
        return new gubg.FSTree.File(path);
    }
}
