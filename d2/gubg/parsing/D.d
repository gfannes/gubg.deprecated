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
        writefln("fp = %s", fp);
        string[] pathParts = [];
        std.algorithm.copy(splitter(fp, regex("[/\\.]")), pathParts);
//        auto reversePP = retro(pathParts);

//        switch (reversePP.front)
//        {
//            case "d":
//            case "di":
//                reversePP.popFront;
//                break;
//            default:
//                return false;
//                break;
//        }
//        auto mnReverseR = retro(splitter(modName, "\\."));
//        for (; !mnReverseR.empty && !fpReverseR.empty; mnReverseR.popFront, fpReverseR.popFront)
//            if (mnReverseR.front != fpReverseR.front)
//                return false;
//        return mnReverseR.empty;
        return true;
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
    }
}
