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
            command = new InsertCommand(node, "end");
	    break;
	case 'o':
            command = new InsertCommand(node, "after");
	    break;
	case 'u':
            command = new InsertCommand(node, "before");
	    break;
// 	case 'r':
//             command = new InsertCommand(node, "replace");
// 	    break;
	default:
	    _input.reset;
	    return false;
	    break;
	}
	_input.clear;
	return (command !is null);
    }

private:
    class InsertCommand: ICommand
    {
	this (DNode node, char[] where)
	{
            _node = node;
            _where = where;
	}
	bool execute()
	{
	    puts("Creating at {}", _where);

            // Find the ix where the newNode should be created
            uint ix;
	    switch (_where)
	    {
	    case "end":
                ix = _node.nrComponents;
		break;
            case "after":
                DNode parent = cast(DNode)_node.parent;
                if (parent is null)
                    return false;
                if (!indexOfParent!(INode)(ix, _node))
                    return false;
                break;
            default:
                return false;
                break;
	    }
            puts("I will create at ix {}", ix);

            // Create newNode
            DNode newNode;
            if (!createNode(newNode, _node, ix))
                return false;

            _node.replaceComponent(ReplaceMode.Create, ix, newNode);

//             if (newNode !is null)
//                 _mover.setCurrent(newNode);
	    return true;
	}
	bool undo(){return false;}
        DNode _node;
	char[] _where;
    }
    bool createNode(inout DNode newNode, DNode node, uint ix)
    {
	foreach (T; Tuple!(DModule))//, ...
	{
	    T t = cast(T)node;
	    if (t !is null)
	    {
		createNode(newNode, t, ix);
                return (newNode !is null);
		break;
	    }
	}
        return false;
    }
    void createNode(inout DNode newNode, DModule node, uint ix)
    {
	puts("Creating new node for DModule");

        char[][] options = ["module", "import", "class"];
        alias Tuple!(DModuleDeclaration, DImportDeclaration, DClassDeclaration) Types;
        
        uint selection;
        if (!_ui.selectString(selection, "Select the declaration you want to create:", options, false))
            return;

	foreach (i, T; Types)
	{
            if (selection == i)
            {
                T t;
                createNode(t);
                newNode = t;
                break;;
            }
        }
    }

    void createNode(inout DImportDeclaration node)
    {
        char[] name;
        if (!_ui.getString(name, "Import: "))
            return;
        node = new DImportDeclaration(name);
    }
    void createNode(inout DModuleDeclaration node)
    {
        char[] name;
        if (!_ui.getString(name, "Module: "))
            return;
        node = new DModuleDeclaration(name);
    }
    void createNode(inout DClassDeclaration node)
    {
        node = new DClassDeclaration();
    }

    BufferedInput _input;
    IOutput _output;
    IMover _mover;
    UI _ui;
}
