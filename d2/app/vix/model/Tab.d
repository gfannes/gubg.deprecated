module vix.model.Tab;

import vix.Exit;

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
import std.utf;

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

    void setDisplayMode(DisplayMode dm){displayMode_ = dm;}
    string getContentPattern(){return contentPattern_;}
    void refresh(ExpandStrat expandStrat)
    {
        folder_.expand(creator_, expandStrat, true);
        switch (expandStrat)
        {
            case ExpandStrat.Recursive:
                //We expanded recursively, lets prune empty dirs too
                while (true)
                {
                    Folder[] emptyFolders;
                    foreach (ref Folder folder; folder_)
                    {
                        if (folder.childs.empty)
                            emptyFolders ~= folder;
                    }
                    if (emptyFolders.empty)
                        break;
                    foreach (folder; emptyFolders)
                        folder.remove;
                }
                setDisplayMode(DisplayMode.Files);
                break;
            case ExpandStrat.Shallow:
                setDisplayMode(DisplayMode.Mixed);
                break;
        }
        updateChilds_;
    }
    string getPath(){return folder_.path;}
    void moveToRoot()
    {
        if (folder_.parent)
            activate(folder_.parent);
    }
    enum DisplayMode {Mixed, Files};
    FSTree[] getChilds(ref uint focusIX, ref DisplayMode dm)
    {
        focusIX = focusIX_;
        dm = displayMode_;
        return childs_;
    }
    void setFolder(string folder)
    {
        activate(Folder.createRecursive(folder, creator_));
    }
    void activate(FSTree tree)
    {
        try
        {
            auto folder = cast(Folder)tree;
            if (!(folder is null))
            {
                folder.expand(creator_, ExpandStrat.Shallow);
                folder_ = folder;
                filter_ = "";
                focus_ = getFocusForFolder_(folder.path);
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
        catch (std.file.FileException){reportError(Format.immediate("Could not activate %s", tree.path));}
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
        setFocus_(childs_[focusIX_].path);
    }
    void deleteFocus()
    {
        try
        {
            string newFocus = (childs_.length > focusIX_+1 ? childs_[focusIX_+1].path : (focusIX_ > 0 ? childs_[focusIX_-1].path : ""));
            std.file.remove(childs_[focusIX_].path);
            focus_ = newFocus;
            refresh(ExpandStrat.Shallow);
        }
        catch (std.file.FileException)
        {
            reportError(Format.immediate("Could not delete %s", childs_[focusIX_].path));
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
    void setFocus(uint focusIX)
    {
        setFocus_((focusIX < childs_.length ? childs_[focusIX].path : ""));
    }
    const(string[]) getSelection() const {return selection_;}
    void addToSelection(string selected){selection_ ~= selected;}

    private:
    void updateChilds_()
    {
        try
        {
            FSTree[] newChilds;
            //Append only the non-hidden files and folders which match the filter_ (case insensitive)
            auto reStr = (filter_.empty ? "." : filter_);
            switch (displayMode_)
            {
                case DisplayMode.Mixed:
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
                                    newChilds ~= child;
                            }
                            version (UseRegExp)
                            {
                                if (re.test(child.name))
                                    newChilds ~= child;
                            }
                        }
                    //Sort the childs using localCmp as criterion
                    bool localCmpMixed(FSTree lhs, FSTree rhs)
                    {
                        //First Folders, then Files
                        if (cast(Folder)lhs && cast(gubg.FSTree.File)rhs)
                            return true;
                        if (cast(gubg.FSTree.File)lhs && cast(Folder)rhs)
                            return false;
                        //If the type results in a tie, sort alphabetically
                        return lhs.name < rhs.name;
                    }
                    sort!(localCmpMixed)(newChilds);
                    break;

                case DisplayMode.Files:
                    version (UseRegex)
                        auto re = regex(reStr);
                    version (UseRegExp)
                        auto re = RegExp(reStr);
                    foreach (ref gubg.FSTree.File file; folder_)
                        if (file.name[0] != '.')
                        {
                            version (UseRegex)
                            {
                                auto m = match(file.path, re);
                                if (!m.empty)
                                    newChilds ~= file;
                            }
                            version (UseRegExp)
                            {
                                if (re.test(file.path))
                                    newChilds ~= file;
                            }
                        }
                    //Sort the childs using localCmp as criterion
                    bool localCmpFiles(FSTree lhs, FSTree rhs)
                    {
                        return lhs.path < rhs.path;
                    }
                    sort!(localCmpFiles)(newChilds);
                    break;
            }
            childs_ = newChilds;
        }
        catch (std.regexp.RegExpException){}
        updateFocus_;
    }
    void setFocus_(string focus)
    {
        focus_ = focus;
        updateFocus_;
    }
    //Tries to update focusIX_ based on focus_
    void updateFocus_()
    {
        if (childs_.empty)
        {
            focus_ = "";
            focusIX_ = 0;
            return;
        }
        bool foundFocus = false;
        foreach (uint ix, child; childs_)
        {
            if (focus_ == child.path)
            {
                foundFocus = true;
                focusIX_ = ix;
                break;
            }
        }
        if (!foundFocus)
        {
            focus_ = childs_[0].path;
            focusIX_ = 0;
        }
        focusPerFolder_[folder_.path] = focus_;
    }

    Creator creator_;
    Folder folder_;
    FSTree[] childs_;
    string focus_ = "";
    uint focusIX_ = 0;
    string getFocusForFolder_(string path)
    {
        if (path in focusPerFolder_)
            return focusPerFolder_[path];
        return "";
    }
    string[string] focusPerFolder_;
    DisplayMode displayMode_ = DisplayMode.Mixed;
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
        reExtension_ = RegExp("\\.(d|rb|cpp|h|hpp|c|txt|xml|java|pl)$");
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
            try
            {
                auto content = readText!(string)(path);
                if (content.length > 100_000 || !reContentPattern_.test(content))
                    return null;
            }
            catch (std.file.FileException){reportError(Format.immediate("Could not read file %s", path));}
            catch (std.utf.UtfException){reportError(Format.immediate("Invalid UTF in file %s", path));}
        }
        return new gubg.FSTree.File(path);
    }
}
