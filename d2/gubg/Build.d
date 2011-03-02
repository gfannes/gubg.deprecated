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

enum SourceType {D, Cpp};
SourceType guessSourceType(string filepath)
{
    switch (getExt(filepath))
    {
        case "d": return SourceType.D; break;
        case "cpp": return SourceType.Cpp; break;
    }
}

abstract class Options
{
    void addOption(string option)
    {
        options_ ~= option;
    }
    protected:
    string[] options_;
}

class Compile: Options
{
    this(string sourceFilepath)
    {
        sourceFilepath_ = sourceFilepath;
        sourceType_ = guessSourceType(sourceFilepath_);
    }
    void setObjectFilepath(string objectFilepath)
    {
        objectFilepath_ = objectFilepath;
    }

    bool execute(bool verbose = true)
    {
        Format format;
        switch (sourceType_)
        {
            case SourceType.D:
                format("dmd -c -version=phobos -of\"%s\" \"%s\"", objectFilepath(), sourceFilepath_);
                format.delimiter = " ";
                foreach (includePath; includePaths_)
                    format("-I\"%s\"", includePath);
                foreach (v; versions_)
                    format("-version=%s", v);
                foreach (option; options_)
                    format(option);
                break;
            case SourceType.Cpp:
                format("g++ -std=c++0x -c -o\"%s\" \"%s\"", objectFilepath(), sourceFilepath_);
                format.delimiter = " ";
                foreach (includePath; includePaths_)
                    format("-I\"%s\"", includePath);
                foreach (v; versions_)
                    format("-D%s", v);
                foreach (option; options_)
                    format(option);
                break;
        }

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
    SourceType sourceType_;
    string objectFilepath_;
    string[] includePaths_;
    string[] versions_;
}

class Link: Options
{
    this (SourceType sourceType)
    {
        sourceType_ = sourceType;
    }

    abstract string extraSettings();

    bool execute(bool verbose)
    {
        Format format;
        version (Posix)
        {
            format.delimiter = " ";
            switch (sourceType_)
            {
                case SourceType.D: format("dmd"); break;
                case SourceType.Cpp: format("g++ -std=c++0x"); break;
            }
            format(extraSettings());
            foreach (objectFilepath; objectFilepaths_)
                format("\"" ~ objectFilepath ~ "\"");
        }
        version (Win32)
        {
            format.delimiter = " ";
            format("d-link");
            {
                Format objectFiles;
                objectFiles.delimiter = "+";
                foreach (objectFilepath; objectFilepaths_)
                    objectFiles("\"" ~ objectFilepath ~ "\"");
                format(objectFiles.toString);
            }
            format.delimiter = ", ";
            format(extraSettings());
        }
        foreach (option; options_)
            format(option);

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
    SourceType sourceType_;
}
class LinkExecutable: Link
{
    this(string exeName, SourceType sourceType)
    {
        exeName_ = exeName;
        super(sourceType);
    }

    //Abstract interface from Link
    string extraSettings()
    {
        Format format;
        version (Posix)
        {
            switch (sourceType_)
            {
                case SourceType.D:
                    format("-of\"%s\"", exeName_);
                    format.delimiter = " ";
                    format("-L-Map=\"%s.map\"", exeName_);
                    foreach (libName; libraries)
                        format("-L-l%s", libName);
                    break;
                case SourceType.Cpp:
                    format("-o\"%s\"", exeName_);
                    format.delimiter = " ";
                    //format("-L-Map=\"%s.map\"", exeName_);
                    foreach (libName; libraries)
                        format("-l%s", libName);
                    break;
            }
        }
        version (Win32)
        {
            format("\"%s\", , ", exeName_);
            {
                Format libFiles;
                libFiles.delimiter = "+";
                foreach (libName; libraries)
                    libFiles("%s.lib", libName);
                format(libFiles.toString);
            }
        }

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
    this(string libName, SourceType sourceType)
    {
        libName = libName_;
        super(sourceType);
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
