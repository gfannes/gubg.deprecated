module rinle.controller.d;

import rinle.controller.interfaces;
import rinle.controller.bufferedInput;
import rinle.controller.ui;
import rinle.model.d.d;

import tango.core.Tuple;

import gubg.puts;

class DCommander: ICommander
{
    this (BufferedInput input, IOutput output, IMover mover)
    {
	_input = input;
	_output = output;
	_mover = mover;
	_ui = new UI(_input, _output);
    }

    // ICommander methods
    bool getCommand(inout ICommand command)
    {
	DNode node = cast(DNode)_mover.current;
	if (node is null)
	    return false;
	int key;
	switch (key = _input.getKey)
	{
	case 'i':
	    createInsertCommand(command, node, "end");
	    break;
	case 'o':
	    createInsertCommand(command, node, "after");
	    break;
	case 'u':
	    createInsertCommand(command, node, "before");
	    break;
	case 'r':
	    createInsertCommand(command, node, "replace");
	    break;
	default:
	    _input.reset;
	    return false;
	    break;
	}
	_input.clear;
	return (command !is null);
    }

private:
    void createInsertCommand(inout ICommand command, DNode node, char[] where)
    {
	uint ix;
	switch (where)
	{
	case "end":
	    ix = node.nrComponents;
	    break;
	case "after":
	    DNode parent = cast(DNode)node.parent;
	    if (parent is null)
		return;
	    if (!indexOfParent!(INode)(ix, node))
		return;
	    break;
	default:
	    return;
	    break;
	}
	createInsertCommand(command, node, ix);
    }
    void createInsertCommand(inout ICommand command, DNode node, uint ix)
    {
	foreach (type; Tuple!(DModule))//, ...
	{
	    type t = cast(type)node;
	    if (t !is null)
	    {
		createInsertCommand(command, t, ix);
		break;
	    }
	}
    }
    void createInsertCommand(inout ICommand command, DModule node, uint ix)
    {
	puts("Creating insert command for DModule");
    }

    BufferedInput _input;
    IOutput _output;
    IMover _mover;
    UI _ui;
}
