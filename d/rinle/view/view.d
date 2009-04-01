module rinle.view.view;

import rinle.view.nodeInfo;
import rinle.model.interfaces;

class View
{
    this (INode base)
        {
            mBase = base;
            mCurrent = base;
            mMgr = new Manager;
        }

private:
    INode mBase;
    INode mCurrent;
    Manager mMgr;
}
