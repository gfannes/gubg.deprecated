module gubg.Build;

import gubg.Format;
import std.process;
import std.path;
import std.stdio;

string objectExtension()
{
	version (Posix) return "o";
	version (Win32) return "obj";
}

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
        format("dmd -c -version=phobos -of\"%s\" \"%s\"", objectFilepath(), sourceFilepath_);
        format.delimiter = " ";
        foreach (includePath; includePaths_)
            format("-I\"%s\"", includePath);
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
        return addExt(sourceFilepath_, objectExtension());
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
            format("\"" ~ objectFilepath ~ "\"");

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

    //Abstract interface from Link
    string extraSettings()
    {
        Format format;
        format("-of\"%s\"", exeName_);
        format.delimiter = " ";
        version (Posix) format("-L-Map=\"%s.map\"", exeName_);
        foreach (libName; libraries)
            format("-L-l%s", libName);

        return format.toString();
    }
    
    void addLibrary(string libName)
    {
        libraries ~= libName;
    }

    private:
    string exeName_;
    string[] libraries;
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
    import std.file;
    void main()
    {
        auto objectFile = "Build.o";
        auto compile = new Compile("Build.d");
        compile.addIncludePath("/home/gfannes/gubg/d2");
        if (exists(objectFile))
            std.file.remove(objectFile);
        assert(!exists(objectFile));
        scope (exit) if (exists(objectFile)) std.file.remove(objectFile);
        compile.execute;
        assert(exists(objectFile));
    }
}
