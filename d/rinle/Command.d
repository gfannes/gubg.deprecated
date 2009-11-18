module rinle.Command;

static import gubg.patterns.Command;

interface ICommand: gubg.patterns.Command.ICommand
{
    bool undoable();
}