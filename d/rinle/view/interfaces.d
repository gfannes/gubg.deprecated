module rinle.view.interfaces;

import rinle.model.interfaces;

import gubg.ui;

interface IView
{
    void hide(bool yes);
    void setOutput(IOutput output);
    void show();
    void setCurrent(INode current);
}
