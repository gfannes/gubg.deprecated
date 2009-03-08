module gubg.patterns.command;

// The command pattern

interface ICommand
{
    bool execute();
    bool undo();
}

version (UnitTest)
{
    class Test: ICommand
    {
	bool execute(){return true;}
	bool undo(){return false;}
    }

    int main()
    {
	scope test = new Test;
	ICommand command = test;
	test.execute;
	test.undo;
	return 0;
    }
}