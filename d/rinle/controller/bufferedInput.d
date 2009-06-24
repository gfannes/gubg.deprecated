module rinle.controller.bufferedInput;

import gubg.ui;

class BufferedInput: IInput
{
    this (IInput input)
    {
	_input = input;
    }

    // IInput methods
    int getKey()
    {
	int key;
	if (_toBeUsed.length > 0)
	{
	    key = _toBeUsed[0];
	    _toBeUsed = _toBeUsed[1 .. $];
	} else
	    key = _input.getKey();
	_alreadyPressed ~= [key];
	return key;
    }
    void clearKeyBuffer()
    {
	_toBeUsed = [];
	_input.clearKeyBuffer;
    }    

    void reset()
    {
	_toBeUsed = _alreadyPressed;
	clear();
    }
    void clear()
    {
	_alreadyPressed.length = 0;
    }

private:
    IInput _input;
    int[] _toBeUsed;
    int[] _alreadyPressed;
}

version (UnitTest)
{
    import gubg.ncurses;
    import gubg.puts;

    void main()
    {
	int[] firstKeys;
	int[] secondKeys;
	int[] thirdKeys;
	{
	    scope nc = new NCurses;
	    scope bi = new BufferedInput(nc);
	    for (uint i = 0; i < 3; ++i)
		firstKeys ~= [bi.getKey];
	    bi.reset;
	    for (uint i = 0; i < 3; ++i)
		secondKeys ~= [bi.getKey];
	    bi.clear;
	    for (uint i = 0; i < 3; ++i)
		thirdKeys ~= [bi.getKey];
	}
	puts("firstKeys = {}", firstKeys);
	puts("secondKeys = {}", secondKeys);
	puts("thirdKeys = {}", thirdKeys);
    }
}
