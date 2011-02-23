module vix.Controller;

import vix.View;
import vix.Model;
import vix.Exit;

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

        version (Posix) bool showBorder = false;
        version (Win32) bool showBorder = true;
        canvas_ = new SDLCanvas(width, height, showBorder);
        canvas_.setTitle("ViX: The Vi-Inspired file eXplorer", "ViX");

        string homeFolder;
        version (Posix) homeFolder = "/home/gfannes";
        version (Win32) homeFolder = "w:\\";
        model_ = new Model(homeFolder);
        model_.setCommandGetter(&getCommand);
        view_ = new View(model_, canvas_);

        reSearch = RegExp("^/(.*)");
    }

    bool quit() const
    {
        return quit_ || !model_.hasTabs();
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
                    {
                        command_ = "";
                        mode_ = Mode.Command;
                    }
                }
                else
                    currentTab_.setFilter("");
                break;
            case Mode.Command:
                if (!command_.empty)
                {
                    if (allowSwitchToCommandMode)
                        command_ = "";
                    else
                        mode_ = Mode.Filter;
                }
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
            case Key.PageUp:
                             currentTab_.moveFocus(Tab.Movement.PageUp); return;
                             break;
            case Key.PageDown:
                             currentTab_.moveFocus(Tab.Movement.PageDown); return;
                             break;
            case Key.Delete:
                             currentTab_.deleteFocus; return;
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
                           if (1 == model_.getTabs().length)
                               quit_ = true;
                           else
                           {
                               uint tabIX;
                               if (model_.removeTab(currentTab_, tabIX))
                                   view_.setCurrentTab(tabIX);
                           }
                           break;
                    case "i": mode_ = Mode.Filter; break;
                    case "r": currentTab_.refresh(ExpandStrat.Shallow); break;
                    case "R": currentTab_.refresh(ExpandStrat.Recursive); break;
                    default:
                              if ('\n' == command_[$-1])
                              {
                                  if (command_.length > 2 && command_[0 .. 1] == "g")
                                  {
                                      //Go to a location
                                      auto location = command_[1 .. $-1];
                                      version (Posix)
                                      {
                                          currentTab_.setFolder(location);
                                      }
                                      version (Win32)
                                      {
                                          if (location.length == 2 && location[1] == ":")
                                              location ~= std.path.sep;
                                          currentTab_.setFolder(location);
                                      }
                                  }
                                  else if (command_.length > 2 && command_[0 .. 1] == "n")
                                  {
                                      //Create a new file
                                      string filename = command_[1 .. $-1];
                                      auto filepath = std.path.join(currentTab_.getPath(), filename);
                                      if (std.file.exists(filepath))
                                          reportError(Format.immediate("File or directory %s already exists", filepath));
                                      else
                                      {
                                          std.file.write(filepath, "");
                                          currentTab_.refresh(ExpandStrat.Shallow);
                                          currentTab_.setFocus(filepath);
                                      }
                                  }
                                  else if (command_.length > 2 && command_[0 .. 2] == ":t")
                                  {
                                      //Opening a new tab
                                      string folder = currentTab_.getPath;
                                      if (4 == command_.length)
                                          folder = Format.immediate("%s:\\", command_[2]);
                                      uint tabIX = model_.insertTab(new Tab(folder), currentTab_);
                                      mode_ = Mode.Filter;
                                      view_.setCurrentTab(tabIX);
                                  }
                                  if (reSearch.test(command_))
                                  {
                                      //Opening a new search tab
                                      auto contentSearch = reSearch[0][1 .. $];
                                      if (!contentSearch.empty)
                                      {
                                          writefln("Search regexp: %s", contentSearch);
                                          model_.insertTab(new Tab(currentTab_.getPath, contentSearch), currentTab_);
                                          mode_ = Mode.Filter;
                                          view_.setCurrentTab(model_.getTabs().length-1);
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
    RegExp reSearch;
    bool quit_ = false;
    SDLCanvas canvas_;
    FewTimes fastProcessing_;
    View view_;
    Model model_;
}
