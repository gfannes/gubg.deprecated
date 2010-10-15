module gubg.parsing.D;

import std.file;
import std.regexp;
import std.regex;
import std.range;
import std.algorithm;
import std.stdio;

class DParser
{
    static bool fileMatchesModule(string fp, string modName)
    {
        //Create a range that iterates backwards over the parts of fp
        auto reverseFpParts = retro(std.algorithm.copy(splitter(fp, regex("[/\\.]")), appender!(string[])).data);
        //Check if the last part of fp is a known D extension
        switch (reverseFpParts.front)
        {
            case "d":
            case "di":
                reverseFpParts.popFront;
                break;
            default:
                return false;
                break;
        }
        //Create a range that iterates backwards over the parts of modName
        auto reverseModnameParts = retro(std.algorithm.copy(splitter(modName, regex("\\.")), appender!(string[])).data);
        //Check if the parts from both ranges match
        for (; !reverseModnameParts.empty && !reverseFpParts.empty; reverseModnameParts.popFront, reverseFpParts.popFront)
        {
            if (reverseModnameParts.front != reverseFpParts.front)
                return false;
        }
        //If all parts of the modName could be matched, we have a valid match
        return reverseModnameParts.empty;
    }
    class DModule
    {
        this (string filepath)
        {
            filepath_ = filepath;
            content_ = cast(string)std.file.read(filepath_);
        }

        string[] imports()
        {
            string[] res;
            auto re = RegExp("import +([a-zA-Z\\.]+);");
            foreach (m; re.search(content_))
            {
                res ~= m[1];
            }

            return res;
        }

        private:
        string filepath_;
        string content_;
    }
    DModule parse(string filepath)
    {
        auto dm = new DModule(filepath);
        return dm;
    }
}

version (parsing_D)
{
    import std.stdio;
    void main()
    {
        auto p = new DParser;
        writeln(p.parse("/home/gfannes/gubg/d2/gubg/parsing/D.d").imports);
        writeln(DParser.fileMatchesModule("/home/gfannes/gubg/test.d", "gubg.test"));
    }
}
