module vix.Model;

public import vix.model.Tab;
import vix.Exit;

import std.array;

class Model
{
    this(string homeFolder)
    {
        insertTab(new Tab(homeFolder));
    }

    bool hasTabs() const {return !tabs_.empty;}
    Tab[] getTabs()
    {
        return tabs_;
    }
    uint insertTab(Tab newTab, const Tab afterTab = null)
    {
        if (afterTab is null)
        {
            tabs_ ~= newTab;
            return tabs_.length-1;
        }
        Tab[] newTabs;
        bool foundAfterTab;
        uint tabIX;
        foreach (ix, tab; tabs_)
        {
            newTabs ~= tab;
            if (afterTab == tab)
            {
                foundAfterTab = true;
                tabIX = ix+1;
                newTabs ~= newTab;
            }
        }
        if (!foundAfterTab)
        {
            reportError("Could not find tab, I will append it at the end");
            newTabs ~= newTab;
            tabIX = newTabs.length-1;
        }
        tabs_ = newTabs;
        return tabIX;
    }
    bool removeTab(Tab tab, ref uint tabIX)
    {
        bool wasRemoved = false;
        tabIX = 0;
        Tab[] tabs;
        foreach (ix, t; tabs_)
            if (t == tab)
            {
                wasRemoved = true;
                tabIX = ix;
            }
            else
                tabs ~= t;
        if (tabIX > 0)
            --tabIX;
        tabs_ = tabs;
        return wasRemoved;
    }
    void setCommandGetter(string delegate() getter){getCommandDg_ = getter;}
    string getCommand() const
    {
        if (getCommandDg_ is null)
            return "Command getter is not set!";
        return getCommandDg_();
    }

    private:
    Tab[] tabs_;
    string delegate() getCommandDg_;
}
