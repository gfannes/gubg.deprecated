module gubg.parsing.Cpp;

import std.file;
import std.string;
import std.regexp;

import std.stdio;

bool verbose__ = true;

class CppParser
{
    //Returns null if header could not be found in fp
    //else, the required include path is given to find fp by importing header
    static string includePathForHeader(string fp, string header)
    {
        version (Win32)
        {
            string origFp = fp;
            fp = replace(fp, std.path.sep, std.path.altsep);
            header = replace(header, std.path.sep, std.path.altsep);
        }
        //Check if header fits at the end of fp
        auto ix = lastIndexOf(fp, header);
        if (-1 == ix)
            return null;
        if (ix != fp.length-header.length)
            return null;
        version (Win32)
            fp = origFp;
        return fp[0 .. ix-1];
    }
    class CppModule
    {
        this (string filepath)
        {
            filepath_ = filepath;
            content_ = cast(string)std.file.read(filepath_);
        }

        string[] includes()
        {
            string[] res;
            auto re = RegExp("#include +[\"<]([a-zA-Z_][a-zA-Z_\\d\\./]*)[\">]");
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
    CppModule parse(string filepath)
    {
        auto dm = new CppModule(filepath);
        return dm;
    }
}

version (UnitTest)
{
    import std.stdio;
    void main()
    {
        auto p = new CppParser;
        writeln(p.parse("test.cpp").includes);
        version (Posix)
            writefln("Include path: %s", CppParser.includePathForHeader("/home/gfannes/gubg/d2/gubg/parsing/test.hpp", "gubg/parsing/test.hpp"));
        version (Win32)
        {
            writefln("Include path: %s", CppParser.includePathForHeader("h:\\My Dropbox\\gubg\\\\d2\\gubg\\parsing\\test.hpp", "gubg/parsing/test.hpp"));
            writefln("Include path: %s", CppParser.includePathForHeader("h:/My Dropbox/gubg/d2/gubg/parsing/test.hpp", "gubg/parsing/test.hpp"));
        }
    }
}
