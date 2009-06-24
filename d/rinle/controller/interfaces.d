module rinle.controller.interfaces;

public import gubg.patterns.command;
public import rinle.model.interfaces;

interface ICommander
{
    // Get the next command
    bool getCommand(inout ICommand command);
}

interface IController: ICommander
{
    // Show all views
    void show();
}

interface IMover
{
    INode current();
    void setCurrent(INode current);
}
