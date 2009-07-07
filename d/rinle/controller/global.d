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
	_pasteStack = new CircularList!(INode);
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
	    command = new CutCommand(_mover.current);
	    break;
	case 'v':
	    command = new PasteCommand(_mover.current, "after");
	    break;
	case 'V':
	    command = new PasteCommand(_mover.current, "before");
	    break;
	case 'I':
	    command = new PasteCommand(_mover.current, "end");
	    break;
	case 'r':
	    command = new PasteCommand(_mover.current, "here");
	    break;
	case 'z':
	    command = new UndoCommand();
	    break;
	default:
	    // This is the last level, we don't try to delegate to our successor
	    return false;
	    break;
	}
	return true;
    }

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

    class CutCommand: ICommand
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
	    _node.cut;
	    _pasteStack.append(_node);
	    _parent.replaceComponent(ReplaceMode.Set, _ix, null);
	    // Find the new current after the removed node, or before, or the parent
	    INode newCurrent;
	    for (uint i = _ix+1; i < _parent.nrComponents; ++i)
	    {
		newCurrent = _parent.replaceComponent(ReplaceMode.Get, i, null);
		if (newCurrent !is null)
		    break;
	    }
	    if (newCurrent is null)
		for (uint i = _ix; i > 0;)
		{
		    --i;
		    newCurrent = _parent.replaceComponent(ReplaceMode.Get, i, null);
		    if (newCurrent !is null)
			break;
		}
	    if (newCurrent is null)
	    {
		puts("Switching to parent for current");
		newCurrent = _parent;
	    }
	    _mover.setCurrent(newCurrent);
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
		_node.paste;
	    }
	    _mover.setCurrent(_node);
	    return true;
	}
	bool undoable(){return true;}
	INode _parent;		// If _parent !is null, the node was removed
        INode _node;
	uint _ix;
    }

    class PasteCommand: ICommand
    {
	this (INode node, char[] where)
	{
	    _node = node;
	    _where = where;
	    _executed = false;
	}
	bool execute()
	{
	    _parent = _node.parent;
	    if (_parent is null)
	    {
		// If there is no parent, we paste in _node at the end
		_where = "end";
	    } else if (!indexOfParent!(INode)(_ix, _node))
		return false;

	    switch (_where)
	    {
	    case "end":
		_parent = _node;
		_ix = _parent.nrComponents;
		break;
	    case "before":
		break;
	    case "after":
		++_ix;
		break;
	    case "here":
		auto n = _parent.replaceComponent(ReplaceMode.Set, _ix, null);
		if (n !is null)
		    n.cut;
		break;
	    default:
		return false;
		break;
	    }

	    INode node2Paste;
	    if (_pasteStack.size() == 0)
		return false;
	    node2Paste = _pasteStack.removeTail();

	    if (_parent.nrComponents <= _ix)
		_parent.replaceComponent(ReplaceMode.Create, _ix, node2Paste);
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
		_parent.replaceComponent(ReplaceMode.Set, _ix, node2Paste);
		node2Paste.paste;
	    }
	    if ("end" != _where)
		_mover.setCurrent(node2Paste);
	    _executed = true;
	    return true;
	}
	bool undo()
	{
	    if (_executed)
		return false;
	    auto node = _parent.replaceComponent(ReplaceMode.Get, _ix, null);
	    if (node is null)
		return false;
	    node.cut;
	    _pasteStack.append(node);
	    _parent.replaceComponent(ReplaceMode.Set, _ix, null);
	    // Find the new current after the removed node, or before, or the parent
	    INode newCurrent;
	    for (uint i = _ix+1; i < _parent.nrComponents; ++i)
	    {
		newCurrent = _parent.replaceComponent(ReplaceMode.Get, i, null);
		if (newCurrent !is null)
		    break;
	    }
	    if (newCurrent is null)
		for (uint i = _ix; i > 0;)
		{
		    --i;
		    newCurrent = _parent.replaceComponent(ReplaceMode.Get, i, null);
		    if (newCurrent !is null)
			break;
		}
	    if (newCurrent is null)
		newCurrent = _parent;
	    _mover.setCurrent(newCurrent);
	    _parent.compact;
	    return true;
	}
	bool undoable(){return true;}

	INode _node;
	char[] _where;
	INode _parent;
	uint _ix;
	bool _executed;
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
    CircularList!(INode) _pasteStack;
}

version (UnitTest)
{
    void main()
    {
    }
}
