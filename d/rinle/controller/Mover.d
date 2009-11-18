module rinle.controller.Mover;

import rinle.controller.Interfaces;

class Mover: IMover
{
    this (INode current, void delegate(INode current) setCurrent)
    {
	_current = current;
	_setCurrent = setCurrent;
    }

    // IMover methods
    INode current()
    {
	return _current;
    }
    void setCurrent(INode current)
    {
	_current = current;
	_setCurrent(current);
    }

private:
    INode _current;
    void delegate(INode current) _setCurrent;
}