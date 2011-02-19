module vix.Controller;

import vix.View;
import vix.Model;

import gubg.OnlyOnce;
import gubg.graphics.Canvas;
import gubg.graphics.IMUI;

import std.stdio;

bool verbose__ = false;

class Controller
{
    this(uint width, uint height)
    {
        fastProcessing_ = FewTimes(10);

        canvas_ = new SDLCanvas(width, height);

        model_ = new Model;
        model_.setCommandGetter(&getCommand);
        view_ = new View(model_, canvas_);
    }
    
    bool quit() const
    {
        return quit_ || model_.tabs_.empty;
    }
    void process()
    {
        scope ds = canvas_.new DrawScope;
        //Check the keyboard
        {
            auto key = canvas_.imui.getLastKey();
            if (verbose__)
                writefln("Key: %s", key);
            if (Key.None != key)
                appendToCommand_(key);
        }

        //Process the display
        view_.process();

        {
            //Give vix a few frames to get things up and running, then switch to a lower framerate
            if (canvas_.imui.waitForInput((fastProcessing_.isActive() ? 0.01 : 5.0)))
                fastProcessing_.reset();
        }
    }

    private:
    Tab currentTab_()
    {
        return view_.currentTab;
    }
    void resetCommand_()
    {
        switch (mode_)
        {
            case Mode.Filter:
                if (currentTab_.getFilter.empty)
                    mode_ = Mode.Command;
                else
                    currentTab_.setFilter("");
                break;
            case Mode.Command:
                command_ = "";
                break;
        }
    }
    void appendToCommand_(Key key)
    {
        switch (key)
        {
            case Key.Escape: resetCommand_; return; break;
            case Key.Return:
                             //Only in filter mode, <enter> acts the same as "->"
                             if (Mode.Filter != mode_)
                                 break;
            case Key.Right:
                             currentTab_.activateFocus; return;
                             break;
            case Key.Left:
                             currentTab_.moveToRoot; return;
                             break;
            case Key.Up:
                             currentTab_.moveFocus(Tab.Movement.Up); return;
                             break;
            case Key.Down:
                             currentTab_.moveFocus(Tab.Movement.Down); return;
                             break;
            default: break;
        }
        auto c = convertToChar(key);
        switch (mode_)
        {
            case Mode.Filter:
                currentTab_.appendToFilter(c);
                break;
            case Mode.Command:
                command_ ~= c;
                bool commandOK = true;
                switch (command_)
                {
                    case ":qa\n": quit_ = true; break;
                    case ":q\n":
                           if (1 == model_.tabs_.length)
                               quit_ = true;
                           else
                           {
                               Tab[] tabs;
                               uint tabIX;
                               foreach (ix, tab; model_.tabs_)
                                   if (tab == currentTab_)
                                       tabIX = ix;
                                   else
                                       tabs ~= tab;
                               if (tabIX > 0)
                                   --tabIX;
                               model_.tabs_ = tabs;
                               view_.setCurrentTab(tabIX);
                           }
                           break;
                    case ":t\n":
                           model_.tabs_ ~= new Tab;
                           view_.setCurrentTab(model_.tabs_.length-1);
                           break;
                    case "i":
                           mode_ = Mode.Filter;
                           break;
                    default:
                           commandOK = false;
                           break;
                }
                if (commandOK)
                    resetCommand_;
                break;
        }
    }
    string getCommand()
    {
        switch (mode_)
        {
            case Mode.Filter:
                return "Filter: " ~ currentTab_().getFilter;
                break;
            case Mode.Command:
                return "Command: " ~ command_;
                break;
        }
        assert(false);
        return "";
    }

    private:
    enum Mode {Filter, Command};
    Mode mode_ = Mode.Filter;
    string command_ = "";
    bool quit_ = false;
    SDLCanvas canvas_;
    FewTimes fastProcessing_;
    View view_;
    Model model_;
}
