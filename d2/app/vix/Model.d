module vix.Model;

import vix.model.Tab;

public import gubg.FSTree;
import gubg.graphics.IMUI;

class Model
{
    this()
    {
        tabs_ ~= new Tab;
        current_ = tabs_[0];
    }

    //Current
    string getCurrentPath(){return current_.getPath;}
    void moveCurrentToRoot(){current_.moveToRoot;}
    FSTree[] getCurrentChilds(){return current_.getChilds;}
    void setCurrent(Folder folder){return current_.setFolder(folder);}
    string getCurrentFocus() const {return current_.getFocus();}
    const(string[]) getCurrentSelection() const {return current_.getSelection;}
    void addToCurrentSelection(string selected){current_.addToSelection(selected);}

    void resetCommand()
    {
        switch (mode_)
        {
            case Mode.Filter:
                if (current_.getFilter.empty)
                    mode_ = Mode.Command;
                else
                    current_.setFilter("");
                break;
            case Mode.Command:
                command_ = "";
                break;
        }
    }
    void appendToCommand(Key key)
    {
        if (Key.Escape == key)
        {
            resetCommand;
            return;
        }
        auto c = convertToChar(key);
        switch (mode_)
        {
            case Mode.Filter:
                current_.appendToFilter(c);
                break;
            case Mode.Command:
                command_ ~= c;
                switch (command_)
                {
                    case ":q\n": quit_ = true; break;
                    case "i":
                           resetCommand;
                           mode_ = Mode.Filter;
                           break;
                    default: break;
                }
                break;
        }
    }
    string getCommand() const
    {
        switch (mode_)
        {
            case Mode.Filter:
                return "Filter: " ~ current_.getFilter;
                break;
            case Mode.Command:
                return "Command: " ~ command_;
                break;
        }
        assert(false);
        return "";
    }
    bool quit() const {return quit_;}

    private:
    Tab[] tabs_;
    Tab current_;
    enum Mode {Filter, Command};
    Mode mode_ = Mode.Filter;
    string command_ = "";
    bool quit_ = false;
}
