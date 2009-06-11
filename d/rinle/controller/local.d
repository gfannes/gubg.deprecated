module rinle.controller.local;

import rinle.controller.interfaces;
import gubg.ui;

import gubg.puts;

class LocalCommander: ICommander
{
    this (IInput input, IOutput output)
    {
	_input = input;
	_output = output;
    }

    // ICommander methods
    bool getCommand(inout ICommand command)
    {
	int key;
	switch (key = _input.getKey)
	{
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
	default:
	    // This is the last level, we don't try to delegate to our successor
	    return false;
	    break;
	}
	return true;
    }

private:
    IInput _input;
    IOutput _output;
}
