module ulbu.builder;

import gubg.puts;

import ulbu.element;
import ulbu.config;

class Builder
{

    void build(char[] dirName, char[] fileName)
        {
            auto mod = Module.createFrom(dirName, fileName);
            auto conf = Config.createFrom(dirName);

            mod.setLocation(conf.location);
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
