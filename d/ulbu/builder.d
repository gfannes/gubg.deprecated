module ulbu.builder;

import gubg.puts;

class Builder;

import ulbu.element;

class Builder
{
    void build(char[] dirName, char[] fileName)
        {
            auto el = Element.createFrom(dirName, fileName, this);
        }

    Body resolveBody(char[] bodyName)
        {
            return mBodyIndex[bodyName];
        }

private:
    char[][Body] mLocation;
    Body[char[]] mBodyIndex;
}

version (Test)
{
    void main()
    {
        auto builder = new Builder;

        builder.build("test", "test.ulbu");
    }
}
