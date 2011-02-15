module vix.Model;

import vix.model.Tab;

public import gubg.FSTree;

class Model
{
    this()
    {
        tabs_ ~= new Tab;
        current_ = tabs_[0];
        command_ = "";
    }

    //Current
    string getCurrentPath(){return current_.getPath;}
    void moveCurrentToRoot(){current_.moveToRoot;}
    FSTree[] getCurrentChilds(){return current_.getChilds;}
    void setCurrent(Folder folder){return current_.setFolder(folder);}
    string getCurrentFilter() const {return current_.getFilter;}
    void setCurrentFilter(string filter){current_.setFilter(filter);}
    const(string[]) getCurrentSelection() const {return current_.getSelection;}
    void addToCurrentSelection(string selected){current_.addToSelection(selected);}

    //Command
    void resetCommand(){command_ = "";}
    void appendToCommand(char c){command_ ~= c;}
    string getCommand() const {return command_;}

    private:
    Tab[] tabs_;
    Tab current_;
    string command_;
}
