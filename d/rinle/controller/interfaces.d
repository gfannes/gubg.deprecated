module rinle.controller.interfaces;

public import gubg.patterns.command;

interface IController
{
    // Get the next command
    ICommand getCommand();
    // Show all views
    void show();
}
