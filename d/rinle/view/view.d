module rinle.view.view;

import rinle.model.interfaces;

class View
{
    this (INode base)
        {
            mBase = base;
            mCurrent = base;
        }

private:
    INode mBase;
    INode mCurrent;
}
