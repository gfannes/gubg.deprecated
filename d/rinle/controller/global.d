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

import tango.util.container.CircularList;

class GlobalController: IController
{
    this (IInput input, IOutput output, INode model, CircularList!(ICommand) commandHistory, void delegate() quit)
    {
	_unbufferedInput = input;
	_bufferedInput = new BufferedInput(input);
	_ui = new UI(_bufferedInput, output);
	_mover = new Mover(model, &setCurrentDelegate);
	_view = new View(model);
	_quit = quit;
	_view.setOutput(_ui.output);
	_localCommander = new LocalCommander(_bufferedInput, _ui.output, _mover);
	_commandHistory = commandHistory;
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
	case 'x':
	    command = new RemoveCommand(_mover.current);
	    break;
	case 'z':
	    command = new UndoCommand();
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
	bool undoable(){return false;}
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
            if (newCurrent !is null)
                _mover.setCurrent(newCurrent);
	    return true;
	}
	bool undo(){return false;}
	bool undoable(){return false;}
	char[] _dir;
	uint _ix;
    }

    class RemoveCommand: ICommand
    {
	this (INode node)
	{
            _node = node;
	}
	bool execute()
	{
	    if (_parent !is null || !indexOfParent!(INode)(_ix, _node))
		return false;
	    _parent = _node.parent;
	    puts("Removing node");
	    _node.remove;
	    puts("Setting location to null");
	    _parent.replaceComponent(ReplaceMode.Set, _ix, null);
	    // Find the new current after the removed node, or before, or the parent
	    INode newCurrent;
	    for (uint i = _ix+1; i < _parent.nrComponents; ++i)
	    {
		puts("Looking for new current after");
		newCurrent = _parent.replaceComponent(ReplaceMode.Get, i, null);
		if (newCurrent !is null)
		    break;
	    }
	    if (newCurrent is null)
		for (uint i = _ix; i > 0;)
		{
		    --i;
		    puts("Looking for new current before");
		    newCurrent = _parent.replaceComponent(ReplaceMode.Get, i, null);
		    if (newCurrent !is null)
			break;
		}
	    if (newCurrent is null)
	    {
		puts("Parent will be new current");
		newCurrent = _parent;
	    }
	    _mover.setCurrent(newCurrent);
	    puts("Before compact");
	    _parent.compact;
	    return true;
	}
	bool undo()
	{
	    if (_parent is null)
		return false;

	    if (_parent.nrComponents <= _ix)
		_parent.replaceComponent(ReplaceMode.Create, _ix, _node);
	    else
	    {
		if (_parent.replaceComponent(ReplaceMode.Get, _ix, null) !is null)
		{
		    // Create an empty element at the back
		    _parent.replaceComponent(ReplaceMode.Create, _parent.nrComponents, null);
		    for (uint i = _parent.nrComponents; i > _ix+1;)
		    {
			--i;
			auto n = _parent.replaceComponent(ReplaceMode.Set, i-1, null);
			_parent.replaceComponent(ReplaceMode.Set, i, n);
		    }
		}
		// Set the node back
		_parent.replaceComponent(ReplaceMode.Set, _ix, _node);
	    }
	    _mover.setCurrent(_node);
	    return true;
	}
	bool undoable(){return true;}
	INode _parent;		// If _parent !is null, the node was removed
        INode _node;
	uint _ix;
    }

    class UndoCommand: ICommand
    {
	bool execute()
	{
	    if (_commandHistory.isEmpty)
		return false;
	    auto command = _commandHistory.removeTail;
	    if (command is null)
		return false;
	    return command.undo;
	}
	bool undo(){return false;}
	bool undoable(){return false;}
    }

    IView _view;
    IMover _mover;
    void delegate() _quit;
    UI _ui;
    IInput _unbufferedInput;
    BufferedInput _bufferedInput;
    LocalCommander _localCommander;
    CircularList!(ICommand) _commandHistory;
}

version (UnitTest)
{
    void main()
    {
    }
}
