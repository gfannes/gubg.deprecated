module vix.Controller;

import vix.controller.ModeMgr;
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
        version (Posix)
        {
            auto icon = import("resources/icon-64x64.bmp");
            string iconPath = "/tmp/vix-icon.bmp";
            std.file.write(iconPath, icon);
        }
        version (Win32)
        {
            auto icon = import("icon-32x32.bmp");
            string iconPath = "c:/tmp/vix-icon.bmp";
            std.file.write(iconPath, icon);
        }
        canvas_.setIcon(iconPath, Color.white);

        modeMgr_ = new ModeMgr(this);

        string homeFolder;
        version (Posix) homeFolder = "/home/gfannes";
        version (Win32) homeFolder = "w:\\";
        model_ = new Model(homeFolder);
        model_.setCommandGetter(&getCommandCallback);
        view_ = new View(model_, canvas_);
    }

    //Quitting functionality
    bool quit() const
    {
        return quit_ || !model_.hasTabs();
    }
    void setQuit()
    {
        reportMessage("I have to quit.");
        quit_ = true;
    }

    //Tab functionality
    Tab currentTab()
    {
        return view_.currentTab;
    }
    void removeCurrentTab()
    {
        uint tabIX;
        if (!model_.removeTab(currentTab(), tabIX))
        {
            reportError("Current tab could not be removed");
            return;
        }
        if (!view_.setCurrentTab(tabIX))
            setQuit();
    }
    uint insertTab(Tab newTab, Tab afterTab)
    {
        return model_.insertTab(newTab, afterTab);
    }
    void setCurrentTab(int tabIX)
    {
        view_.setCurrentTab(tabIX);
    }
    int currentTabIX()
    {
        return view_.currentTabIX();
    }

    //Processing called by the IMUI mainloop
    void process()
    {
        scope ds = canvas_.new DrawScope;
        //Check the keyboard
        {
            auto key = canvas_.imui.getLastKey();
            if (verbose__)
                writefln("Key: %s", key);
            if (Key.None != key)
                modeMgr_.processEvent(key);
        }

        //Check if we have to quit before attempting to draw
        if (quit_)
            return;

        //Process the display
        view_.process();

        {
            //Give vix a few frames to get things up and running, then switch to a lower framerate
            if (canvas_.imui.waitForInput((fastProcessing_.isActive() ? 0.01 : 5.0)))
                fastProcessing_.reset();
        }
    }

    private:
    string getCommandCallback()
    {
        return modeMgr_.getCommand();
    }

    private:
    ModeMgr modeMgr_;
    RegExp reSearch;
    bool quit_ = false;
    SDLCanvas canvas_;
    FewTimes fastProcessing_;
    View view_;
    Model model_;
}
