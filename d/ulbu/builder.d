module ulbu.builder;

import gubg.puts;

import ulbu.element;
import ulbu.config;

class Builder
{

    void build(char[] dirName, char[] fileName)
        {
            auto el = Element.createFrom(dirName, fileName);
        }
}

version (Test)
{
    void main()
    {
        auto builder = new Builder;

        builder.build("test", "test.ulbu");
    }
}
