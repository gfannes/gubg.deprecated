module rinle.command;

static import gubg.patterns.command;

interface ICommand: gubg.patterns.command.ICommand
{
    bool undoable();
}
