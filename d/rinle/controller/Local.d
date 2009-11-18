module rinle.controller.Local;

import rinle.controller.Interfaces;
import rinle.controller.BufferedInput;
import rinle.controller.D;
import gubg.UI;

import gubg.Puts;

class LocalCommander: ICommander
{
    this (BufferedInput input, IOutput output, IMover mover)
    {
	_input = input;
	_output = output;
	_mover = mover;
	_commanders = [new DCommander(_input, _output, _mover)];
    }

    // ICommander methods
    bool getCommand(inout ICommand command)
    {
	foreach (commander; _commanders)
	    if (commander.getCommand(command))
		return true;
	return false;
// 	int key;
// 	switch (key = _input.getKey)
// 	{
// 	case 'q':
// 	    command = new QuitCommand;
// 	    break;
// 	case 'w':
// 	    command = new MoveCommand("up");
// 	    break;
// 	case 's':
// 	    command = new MoveCommand("down");
// 	    break;
// 	case 'a':
// 	    command = new MoveCommand("out");
// 	    break;
// 	case 'd':
// 	    command = new MoveCommand("in");
// 	    break;
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
// 	default:
// 	    // This is the last level, we don't try to delegate to our successor
// 	    return false;
// 	    break;
// 	}
// 	return true;
    }

private:
    BufferedInput _input;
    IOutput _output;
    ICommander[] _commanders;
    IMover _mover;
}