module gubg.parsing.D;

import std.file;
import std.path;
import std.regexp;
import std.regex;
import std.range;
import std.algorithm;
import std.stdio;

class DParser
{
    //Returns null if modName could not be found in fp
    //else, the required include path is given to find fp by importing modName
    static string includePathForModule(string fp, string modName)
    {
        auto drive = getDrive(fp);
        if (drive)
            fp = fp[drive.length .. $];
        //Create a range that iterates backwards over the parts of fp
        auto fpParts = appender!(string[]);
        if (std.path.altsep.empty)
            std.algorithm.copy(std.algorithm.splitter(fp, std.path.sep), fpParts);
        else
            std.algorithm.copy(std.regex.splitter(fp, regex("[/\\\\]")), fpParts);
        auto reverseFpParts = retro(fpParts.data);
        auto filename = reverseFpParts.front;
        auto extension = getExt(filename);
        reverseFpParts.front = basename(filename, "." ~ extension);

        //Check if the last part of fp is a known D extension
        if (std.algorithm.find(["d", "di", "apd"], extension).empty)
            return null;
        //Create a range that iterates backwards over the parts of modName
        auto reverseModnameParts = retro(std.algorithm.copy(std.algorithm.splitter(modName, "."), appender!(string[])).data);

        //Check if the parts from both ranges match
        for (; !reverseModnameParts.empty && !reverseFpParts.empty; reverseModnameParts.popFront, reverseFpParts.popFront)
            if (reverseModnameParts.front != reverseFpParts.front)
                return null;

        //If not all parts of the modName could be matched, we don't have a valid match
        if (!reverseModnameParts.empty)
            return null;

        //We have a valid match, join the remaining parts from reverseFpParts into the include path
        auto includePath = appender!(char[]);
        if (drive)
            includePath.put(drive);
        for (; !reverseFpParts.empty; reverseFpParts.popBack)
        {
            auto part = reverseFpParts.back;
            if ("" != part)
            {
                includePath.put(std.path.sep);
                includePath.put(part);
            }
        }
        return cast(string)includePath.data;
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

version (UnitTest)
{
    import std.stdio;
    void main()
    {
        auto p = new DParser;
        writeln(p.parse("D.d").imports);
        if (std.path.sep == "/")
            writefln("Include path: %s", DParser.includePathForModule("/home/gfannes/gubg/test.d", "gubg.test"));
        if (std.path.sep == "\\")
        {
            writefln("Include path: %s", DParser.includePathForModule("h:\\My Dropbox\\gubg\\test.d", "gubg.test"));
            writefln("Include path: %s", DParser.includePathForModule("h:/My Dropbox/gubg/test.d", "gubg.test"));
        }
    }
}
