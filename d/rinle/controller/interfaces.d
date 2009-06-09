module rinle.controller.interfaces;

public import gubg.patterns.command;

interface ICommander
{
    // Get the next command
    // Returns true if the user inputs made some basic understanding, meaning they should not be reused anymore
    // Returns false if the user inputs can be reused for the next commander in line
    // Always check if command !is null; some first user input could have made sense, but actual command creation might have been cancelled or so along the way
    bool getCommand(inout ICommand command);
}

interface IController: ICommander
{
    // Show all views
    void show();
}
