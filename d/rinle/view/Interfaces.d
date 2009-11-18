module rinle.view.Interfaces;

import rinle.model.Interfaces;

import gubg.UI;

interface IView
{
    void hide(bool yes);
    void setOutput(IOutput output);
    void show();
    void setCurrent(INode current);
}