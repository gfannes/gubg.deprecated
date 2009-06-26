module rinle.controller.global;

import rinle.controller.interfaces;
import rinle.controller.ui;
import rinle.model.interfaces;
import rinle.view.view;
import rinle.controller.local;
import rinle.controller.bufferedInput;
import rinle.controller.mover;

import gubg.ui;

import gubg.puts;

class GlobalController: IController
{
    this (IInput input, IOutput output, INode model, void delegate() quit)
    {
	_unbufferedInput = input;
	_bufferedInput = new BufferedInput(input);
	_ui = new UI(_bufferedInput, output);
	_mover = new Mover(model, &setCurrentDelegate);
	_view = new View(model);
	_quit = quit;
	_view.setOutput(_ui.output);
	_localCommander = new LocalCommander(_bufferedInput, _ui.output, _mover);
    }

    // IController methods
    bool getCommand(inout ICommand command)
    {
	_bufferedInput.clear;
	if (_localCommander.getCommand(command))
	    return true;
	return getGlobalCommand(command);
    }
    void show()
    {
	clear;
	_view.show();
    }

private:
    void setCurrentDelegate(INode cur)
    {
	if (cur !is null)
	    _view.setCurrent(cur);
    }
    INode current()
    {
	return _mover.current;
    }

    void clear()
    {
	static IOutput.ISavePoint savePoint;
	if (savePoint is null)
	    savePoint = _ui.output.save;
	else
	    savePoint.restore;
    }

    bool getGlobalCommand(inout ICommand command)
    {
	int key = _bufferedInput.getKey;
	switch (key)
	{
	case 'q':
	    command = new QuitCommand;
	    break;
	case 'w':
	    command = new MoveCommand("up");
	    break;
	case 's':
	    command = new MoveCommand("down");
	    break;
	case 'a':
	    command = new MoveCommand("out");
	    break;
	case 'd':
	    command = new MoveCommand("in");
	    break;
//             case 'i':
//                 command = new InsertCommand("end", _ui);
//                 break;
//             case 'o':
//                 command = new InsertCommand("after", _ui);
//                 break;
//             case 'u':
//                 command = new InsertCommand("before", _ui);
//                 break;
//             case 'r':
//                 command = new InsertCommand("replace", _ui);
//                 break;
	default:
	    // This is the last level, we don't try to delegate to our successor
	    return false;
	    break;
	}
	return true;
    }

//     void insert(char[] location, IUI ui)
//     {
// 	puts("Inserting {}", location);
// 	INode newNode;
// 	switch (location)
// 	{
// 	case "end":
// 	    if (!current.isLeaf && current.create(newNode, current.nrComponents, ui))
// 		current.replaceComponent(ReplaceMode.Create, current.nrComponents, newNode);
// 	    break;
// 	case "after":
// 	    break;
// 	case "before":
// 	    break;
// 	case "replace":
// 	    break;
// 	}
//     }
    class QuitCommand: ICommand
    {
	bool execute()
	{
	    _quit();
	    return true;
	}
	bool undo(){return false;}
    }

    class MoveCommand: ICommand
    {
	this (char[] dir, uint ix = 0)
	{
	    _dir = dir;
	    _ix = ix;
	}
	bool execute()
	{
	    puts("Moving {}", _dir);
	    INode newCurrent;
	    switch (_dir)
	    {
	    case "up":
		if (indexOfParent!(INode)(_ix, current) && _ix > 0)
		    newCurrent = current.parent.replaceComponent(ReplaceMode.Get, --_ix, null);
		break;
	    case "down":
		if (indexOfParent!(INode)(_ix, current) && _ix < current.parent.nrComponents-1)
		    newCurrent = current.parent.replaceComponent(ReplaceMode.Get, ++_ix, null);
		break;
	    case "in":
		current.expand;
		if (current.nrComponents > 0)
		    newCurrent = current.replaceComponent(ReplaceMode.Get, _ix, null);
		break;
	    case "out":
		newCurrent = current.parent;
		break;
	    }
	    _mover.setCurrent(newCurrent);
	    return true;
	}
	bool undo(){return false;}
	char[] _dir;
	uint _ix;
    }

//     class InsertCommand: ICommand
//         {
//             this (char[] location, IUI ui)
//             {
//                 _location = location;
// 		_ui = ui;
//             }
//             bool execute()
//             {
//                 _view.insert(_location, _ui);
//                 return true;
//             }
//             bool undo(){return false;}
//             char[] _location;
// 	    IUI _ui;
//         }

    IView _view;
    IMover _mover;
    void delegate() _quit;
    UI _ui;
    IInput _unbufferedInput;
    BufferedInput _bufferedInput;
    LocalCommander _localCommander;
}

version (UnitTest)
{
    void main()
    {
    }
}
