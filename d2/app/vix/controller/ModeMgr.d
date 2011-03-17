module vix.controller.ModeMgr;

import vix.Controller;
import vix.Model;
import vix.Exit;

import gubg.StateMachine;
import gubg.graphics.IMUI;
import gubg.Format;

interface IMode: IEventHandler!(Key)
{
    string getCommand();
}

class CommandMode: IMode
{
    this (ModeMgr meta, Controller controller)
    {
        meta_ = meta;
        controller_ = controller;
    }

    //IMode API
    bool processEvent(Key key)
    {
        switch (key)
        {
            case Key.Escape:
                if (command_.empty())
                    return false;
                command_ = "";
                if (switchToFilterModeOnEsc_)
                {
                    switchToFilterModeOnEsc_ = false;
                    meta_.changeToFilterMode();
                }
                break;
            case Key.Backspace:
                if (!command_.empty())
                    command_.popBack();
                break;
            default:
                char ch;
                if (!convertToChar(ch, key))
                    return false;
                if (command_.empty() && ':' == ch)
                    break;
                command_ ~= ch;
                tryExecuteCommand_();
                break;
        }
        return true;
    }
    string getCommand(){return "Command: " ~ command_;}

    void switchToFilterModeOnEsc()
    {
        switchToFilterModeOnEsc_ = true;
    }

    private:
    void tryExecuteCommand_()
    {
        switch (command_)
        {
            case "qa\n": controller_.setQuit(); break;
            case "q\n": controller_.removeCurrentTab(); break;
            case "i": meta_.changeToFilterMode(); break;
            case "r": currentTab_().refresh(ExpandStrat.Shallow); break;
            case "R": currentTab_().refresh(ExpandStrat.Recursive); break;
            default:
                      if ('\n' != command_[$-1])
                          return;

                      if (command_.length > 2 && command_[0 .. 1] == "g")
                      {
                          //Go to a location
                          auto location = command_[1 .. $-1];
                          version (Win32)
                          {
                              if (location.length == 2 && location[1 .. 2] == ":")
                                  location ~= std.path.sep;
                          }
                          currentTab_().setFolder(location);
                      }
                      else if (command_.length > 2 && command_[0 .. 1] == "n")
                      {
                          //Create a new file or directory
                          string filename = command_[1 .. $-1];
                          auto path = std.path.join(currentTab_().getPath(), filename);
                          if (std.file.exists(path))
                              reportError(Format.immediate("File or directory %s already exists", path));
                          else
                          {
                              if (path[$-1 .. $] == std.path.sep)
                              {
                                  //Create a directory
                                  std.file.mkdir(path);
                                  path = path[0 .. $-1];
                              }
                              else
                              {
                                  //Create a file
                                  std.file.write(path, "");
                              }
                              currentTab_().refresh(ExpandStrat.Shallow);
                              currentTab_().setFocus(path);
                          }
                      }
                      else if (command_.length > 1 && command_[0 .. 1] == "t")
                      {
                          //Opening a new tab
                          string folder = currentTab_().getPath;
                          if (command_.length > 3)
                          {
                              folder = command_[1 .. $-1];
                              version (Win32)
                              {
                                  if (folder.length == 2 && folder[1 .. 2] == ":")
                                      folder ~= std.path.sep;
                              }
                          }
                          uint tabIX = controller_.insertTab(new Tab(folder), currentTab_());
                          controller_.setCurrentTab(tabIX);
                          meta_.changeToFilterMode();
                      }
                      else if (command_.length > 3 && command_[0 .. 2] == "//")
                      {
                          //Opening a new search tab and fully expand it
                          auto contentSearch = command_[2 .. $-1];
                          if (!contentSearch.empty)
                          {
                              reportMessage(Format.immediate("Search regexp: %s", contentSearch));
                              auto tab = new Tab(currentTab_().getPath, contentSearch);
                              tab.refresh(ExpandStrat.Recursive);
                              uint tabIX = controller_.insertTab(tab, currentTab_());
                              controller_.setCurrentTab(tabIX);
                              meta_.changeToFilterMode();
                          }
                      }
                      else if (command_.length > 2 && command_[0 .. 1] == "/")
                      {
                          //Opening a new search tab
                          auto contentSearch = command_[1 .. $-1];
                          if (!contentSearch.empty)
                          {
                              reportMessage(Format.immediate("Search regexp: %s", contentSearch));
                              uint tabIX = controller_.insertTab(new Tab(currentTab_().getPath, contentSearch), currentTab_());
                              controller_.setCurrentTab(tabIX);
                              meta_.changeToFilterMode();
                          }
                      }
                      else
                          return;
                      break;
        }
        command_ = "";
    }

    Tab currentTab_()
    {
        return controller_.currentTab();
    }

    ModeMgr meta_;
    Controller controller_;
    string command_ = "";
    bool switchToFilterModeOnEsc_ = false;
}
class FilterMode: IMode
{
    this (Controller controller)
    {
        controller_ = controller;
    }

    //IMode API
    bool processEvent(Key key)
    {
        switch (key)
        {
            case Key.Escape:
                if (getFilter_().empty())
                    return false;
                reset();
                break;
            case Key.Return:
                return false;
                break;
            case Key.Backspace:
                currentTab_().popFromFilter();
                break;
            default:
                char ch;
                if (!convertToChar(ch, key))
                    return false;
                if (':' == ch)
                    return false;
                currentTab_().appendToFilter(ch);
                break;
        }
        return true;
    }
    string getCommand()
    {
        return "Filter: " ~ getFilter_();
    }

    void reset()
    {
        setFilter_("");
    }

    private:
    Tab currentTab_()
    {
        return controller_.currentTab();
    }
    string getFilter_()
    {
        return controller_.currentTab().getFilter();
    }
    void setFilter_(string filter)
    {
        controller_.currentTab().setFilter(filter);
    }
    Controller controller_;
}

class ModeMgr: MetaStateMachine!(Key, IMode)
{
    this (Controller controller)
    {
        controller_ = controller;
        commandMode_ = new CommandMode(this, controller);
        filterMode_ = new FilterMode(controller);
        //We start in filter mode
        super(filterMode_);
    }

    //IMode API
    string getCommand()
    {
        return state().getCommand();
    }

    void changeToFilterMode()
    {
        changeState(filterMode_);
    }

    protected:
    bool processEventLocally(Key key)
    {
        switch (key)
        {
            case Key.Escape:
                changeState(commandMode_);
                break;
            case Key.Return:
            case Key.Right:
                currentTab_().activateFocus;
                changeState(filterMode_);
                break;
            case Key.Left:
                currentTab_().moveToRoot;
                changeState(filterMode_);
                break;
            case Key.Up:
                currentTab_().moveFocus(Tab.Movement.Up);
                break;
            case Key.Down:
                currentTab_().moveFocus(Tab.Movement.Down);
                break;
            case Key.PageUp:
                currentTab_().moveFocus(Tab.Movement.PageUp);
                break;
            case Key.PageDown:
                currentTab_().moveFocus(Tab.Movement.PageDown);
                break;
            case Key.Delete:
                currentTab_().deleteFocus;
                break;
            case Key.CtrlPageUp:
                controller_.setCurrentTab(controller_.currentTabIX()-1);
                break;
            case Key.CtrlPageDown:
                controller_.setCurrentTab(controller_.currentTabIX()+1);
                break;
            case Key.F1:  setCurrentTab_(0); filterMode_.reset(); changeState(filterMode_); break;
            case Key.F2:  setCurrentTab_(1); filterMode_.reset(); changeState(filterMode_); break;
            case Key.F3:  setCurrentTab_(2); filterMode_.reset(); changeState(filterMode_); break;
            case Key.F4:  setCurrentTab_(3); filterMode_.reset(); changeState(filterMode_); break;
            case Key.F5:  setCurrentTab_(4); filterMode_.reset(); changeState(filterMode_); break;
            case Key.F6:  setCurrentTab_(5); filterMode_.reset(); changeState(filterMode_); break;
            case Key.F7:  setCurrentTab_(6); filterMode_.reset(); changeState(filterMode_); break;
            case Key.F8:  setCurrentTab_(7); filterMode_.reset(); changeState(filterMode_); break;
            case Key.F9:  setCurrentTab_(8); filterMode_.reset(); changeState(filterMode_); break;
            case Key.F10: setCurrentTab_(9); filterMode_.reset(); changeState(filterMode_); break;
            case Key.F11: setCurrentTab_(10); filterMode_.reset(); changeState(filterMode_); break;
            case Key.F12: setCurrentTab_(11); filterMode_.reset(); changeState(filterMode_); break;

            default:
                          char ch;
                          if (!convertToChar(ch, key))
                              return false;
                          if (':' == ch)
                          {
                              commandMode_.switchToFilterModeOnEsc();
                              changeState(commandMode_);
                              break;
                          }
                          break;
        }
        return true;
    }

    private:
    Tab currentTab_()
    {
        return controller_.currentTab();
    }
    void setCurrentTab_(uint tabIX)
    {
        controller_.setCurrentTab(tabIX);
    }
    Controller controller_;
    CommandMode commandMode_;
    FilterMode filterMode_;
}

version (UnitTest)
{
    void main()
    {
    }
}
