module vix.Controller;

import vix.View;
import vix.Model;

import gubg.OnlyOnce;
import gubg.graphics.Canvas;
import gubg.graphics.IMUI;
import gubg.Format;

import std.regexp;

import std.stdio;

bool verbose__ = false;

class Controller
{
    this(uint width, uint height)
    {
        fastProcessing_ = FewTimes(10);

        canvas_ = new SDLCanvas(width, height, false);

        string homeFolder;
        version (Posix) homeFolder = "/home/gfannes";
        version (Win32) homeFolder = "w:\\";
        model_ = new Model(homeFolder);
        model_.setCommandGetter(&getCommand);
        view_ = new View(model_, canvas_);

        reGoto = RegExp("^g(.)");
        reSearch = RegExp("^/(.*)");
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
    void resetCommand_(bool allowSwitchToCommandMode = false)
    {
        switch (mode_)
        {
            case Mode.Filter:
                if (currentTab_.getFilter.empty)
                {
                    if (allowSwitchToCommandMode)
                        mode_ = Mode.Command;
                }
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
            case Key.Escape: resetCommand_(true); return; break;
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
            case Key.F1: view_.setCurrentTab(0); resetCommand_; return; break;
            case Key.F2: view_.setCurrentTab(1); resetCommand_; return; break;
            case Key.F3: view_.setCurrentTab(2); resetCommand_; return; break;
            case Key.F4: view_.setCurrentTab(3); resetCommand_; return; break;
            case Key.F5: view_.setCurrentTab(4); resetCommand_; return; break;
            case Key.F6: view_.setCurrentTab(5); resetCommand_; return; break;
            case Key.F7: view_.setCurrentTab(6); resetCommand_; return; break;
            case Key.F8: view_.setCurrentTab(7); resetCommand_; return; break;
            case Key.F9: view_.setCurrentTab(8); resetCommand_; return; break;
            case Key.F10: view_.setCurrentTab(9); resetCommand_; return; break;
            case Key.F11: view_.setCurrentTab(10); resetCommand_; return; break;
            case Key.F12: view_.setCurrentTab(11); resetCommand_; return; break;
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
                bool resetCommandAtEnd = true;
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
                           model_.tabs_ ~= new Tab(currentTab_.getPath);
                           mode_ = Mode.Filter;
                           view_.setCurrentTab(model_.tabs_.length-1);
                           break;
                    case "i": mode_ = Mode.Filter; break;
                    case "r": currentTab_.refresh; break;
                    default:
                              if (reGoto.test(command_))
                                  currentTab_.setFolder(Format.immediate("%s:\\", reGoto[0][1]));
                              else if ('\n' == command_[$-1])
                              {
                                  if (reSearch.test(command_))
                                  {
                                      auto contentSearch = reSearch[0][1 .. $];
                                      if (!contentSearch.empty)
                                      {
                                          writefln("Search regexp: %s", contentSearch);
                                          model_.tabs_ ~= new Tab(currentTab_.getPath, contentSearch);
                                          mode_ = Mode.Filter;
                                          view_.setCurrentTab(model_.tabs_.length-1);
                                      }
                                  }
                              }
                              else
                                  resetCommandAtEnd = false;
                              break;
                }
                if (resetCommandAtEnd)
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
    RegExp reGoto;
    RegExp reSearch;
    bool quit_ = false;
    SDLCanvas canvas_;
    FewTimes fastProcessing_;
    View view_;
    Model model_;
}
