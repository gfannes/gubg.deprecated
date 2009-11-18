module rinle.controller.Interfaces;

public import rinle.Command;

public import rinle.model.Interfaces;

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