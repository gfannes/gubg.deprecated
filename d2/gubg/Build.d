module gubg.Build;

import gubg.Format;
import std.process;
import std.path;
import std.stdio;

class Compile
{
    this(string sourceFilepath)
    {
        sourceFilepath_ = sourceFilepath;
    }
    void setObjectFilepath(string objectFilepath)
    {
        objectFilepath_ = objectFilepath;
    }

    bool execute(bool verbose = true)
    {
        Format format;
        format("dmd -c -of%s %s", objectFilepath(), sourceFilepath_);
        format.delimiter = " ";
        foreach (includePath; includePaths_)
            format("-I%s", includePath);
        foreach (v; versions_)
            format("-version=%s", v);

        //Execute the command
        if (verbose)
            writeln(format.toString);
        if (0 != system(format.toString))
            return false;

        return true;
    }
    void addIncludePath(string path)
    {
        includePaths_ ~= path;
    }
    void addVersion(string v)
    {
        versions_ ~= v;
    }
    string objectFilepath()
    {
        if (objectFilepath_)
            return objectFilepath_;
        return addExt(sourceFilepath_, "o");
    }

    private:
    string sourceFilepath_;
    string objectFilepath_;
    string[] includePaths_;
    string[] versions_;
}

class Link
{
    abstract string extraSettings();

    bool execute(bool verbose)
    {
        Format format;
        format.delimiter = " ";
        format("dmd");
        format(extraSettings());
        foreach (objectFilepath; objectFilepaths_)
            format(objectFilepath);

        //Execute the command
        if (verbose)
            writeln(format.toString);
        if (0 != system(format.toString))
            return false;

        return true;
    }

    void addObjectFilepath(string objectFilepath)
    {
        objectFilepaths_ ~= objectFilepath;
    }

    protected:
    string[] objectFilepaths_;
}
class LinkExecutable: Link
{
    this(string exeName)
    {
        exeName_ = exeName;
    }

    string extraSettings()
    {
        return Format.immediate("-of%s", exeName_);
    }

    private:
    string exeName_;
}
class LinkLibrary: Link
{
    this(string libName)
    {
        libName = libName_;
    }

    string extraSettings()
    {
        return Format.immediate("-lib -of%s", libName_);
    }

    private:
    string libName_;
}

version(UnitTest)
{
    void main()
    {
        auto compile = new Compile("Build.d");
        compile.addIncludePath("/home/gfannes/gubg/d2");
        compile.execute;
    }
}
