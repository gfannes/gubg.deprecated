module MetaInfo;

import std.file;
import std.string;
import std.regex;

//Extracts meta-information from a file
class MetaInfo
{
    this(string filepath)
    {
        lines_ = splitlines(cast(string)read(filepath));
    }

    string[] getImports()
    {
        string[] res;
        auto re = regex("^import (.+);");
        foreach (line; lines_)
        {
            auto m = match(line, re);
            if (!m.empty)
                res ~= m.front.captures[1];
        }
        return res;
    }

    private:
    string[] lines_;
}

version(UnitTest)
{
    import std.stdio;
    void main()
    {
        auto mi = new MetaInfo("MetaInfo.d");
        foreach (imp; mi.getImports)
            writeln(imp);
    }
}
