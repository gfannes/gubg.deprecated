module ulbu.primitive;

import ulbu.element;

class Integer: BodyDefinition
{
    this ()
    {
	super([]);
    }

    uint size(){return 4;}
}

class Byte: BodyDefinition
{
    this ()
    {
	super([]);
    }

    uint size(){return 1;}
}

version (Test)
{
    void main()
    {
        auto i = new Integer;
        auto b = new Byte;
    }
}
