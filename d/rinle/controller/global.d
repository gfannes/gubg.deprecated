module rinle.controller.global;

import rinle.controller.interfaces;
import rinle.controller.ui;
import rinle.model.interfaces;
import rinle.view.view;
import rinle.controller.local;
import rinle.controller.bufferedInput;

import gubg.ui;

import gubg.puts;

class GlobalController: IController
{
    this (IInput input, IOutput output, INode model, void delegate() quit)
    {
	_unbufferedInput = input;
	_bufferedInput = new BufferedInput(input);
	_ui = new UI(_bufferedInput, output);
	_model = model;
	_current = model;
	_quit = quit;
	_view = new View(_model);
	_view.setOutput(_ui.output);
	_localCommander = new LocalCommander(_bufferedInput, _ui.output);
    }

    // IController methods
    bool getCommand(inout ICommand command)
    {
	_bufferedInput.clear;
	if (_localCommander !is null)
	    if (_localCommander.getCommand(command))
		return true;
	    else
		_bufferedInput.reset;
	return getGlobalCommand(command);
    }
    void show()
    {
	_view.show();
    }

private:
    void setCurrent(INode current)
    {
	if (current is null)
	    return;
	_current = current;
	_view.setCurrent(_current);
    }

    bool getGlobalCommand(inout ICommand command)
    {
	int key = _ui.input.getKey;
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
// 	    if (!_current.isLeaf && _current.create(newNode, _current.nrComponents, ui))
// 		_current.replaceComponent(ReplaceMode.Create, _current.nrComponents, newNode);
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
		if (indexOfParent!(INode)(_ix, _current) && _ix > 0)
		    newCurrent = _current.parent.replaceComponent(ReplaceMode.Get, --_ix, null);
		break;
	    case "down":
		if (indexOfParent!(INode)(_ix, _current) && _ix < _current.parent.nrComponents-1)
		    newCurrent = _current.parent.replaceComponent(ReplaceMode.Get, ++_ix, null);
		break;
	    case "in":
		_current.expand;
		if (_current.nrComponents > 0)
		    newCurrent = _current.replaceComponent(ReplaceMode.Get, _ix, null);
		break;
	    case "out":
		newCurrent = _current.parent;
		break;
	    }
	    setCurrent(newCurrent);
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
    INode _model;
    INode _current;
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
