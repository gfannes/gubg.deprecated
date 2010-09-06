module gb;

import Collection;
import gubg.Build;
import gubg.FileCache;
import gubg.Format;
import std.stdio;
import std.getopt;
import std.path;
import std.file;
import std.process;
import c_process = std.c.process;

immutable int versionMajor_ = 0;
immutable int versionMinor_ = 0;
immutable int versionBuild_ = 0;

void main(string[] args)
{
    //Short commands
    void printVersion_()
    {
        exitApp(ExitCode.ok, "Version %s.%s.%s", versionMajor_, versionMinor_, versionBuild_);
    }
    void printHelp_()
    {
        exitApp(ExitCode.ok, r"Generic build usage: gb [options] <file-to-build>
Options:
    --version       Prints the version
    --help          Prints this help

Created by Geert Fannes under the GPL.");
    }

    //Parse command line options
    try
    {
    getopt(args,
            "version", &printVersion_,
            "help", &printHelp_);
    } catch (Exception)
    {
        printHelp_();
    }
    //Unshift the executable
    args = args[1 .. $];

    //Get the filepath of the file to build
    string filepath;
    switch (args.length)
    {
        case 0:
            exitApp(ExitCode.error, "You have to specify what file you want to build.");
            break;
        case 1:
            filepath = rel2abs(args[0]);
            if (!exists(filepath))
                exitApp(ExitCode.error, "File \"%s\" does not exist.", filepath);
            if (!isfile(filepath))
                exitApp(ExitCode.error, "File \"%s\" is not a file.", filepath);
            break;
        default:
            exitApp(ExitCode.error, "You can only specify one file to build.");
            break;
    }
    writefln("I will build \"%s\".", filepath);

    //Create the Tree Collection
    auto collection = new DCollection(dirname(filepath));

    //Compile the sources and collect the object filepaths
    string[] objectFilepaths;
    auto fileCache = new FileCache("/tmp/gb");
    foreach (gubg.Tree.File file; collection)
    {
        //The source file that needs to be compiled
        string sourceFilepath = file.path;

        //Collect all information about the source file
        auto fi = FileInfo(addExt(sourceFilepath, "o"));
        if (filepath == sourceFilepath)
            fi.add("version", "UnitTest");
        fi.add("includePath", "/home/gfannes/gubg/d2");
        fi.addFile(sourceFilepath);

        //Compile, if necessary
        bool compile(FileInfo fi)
        {
            auto c = new Compile(sourceFilepath);
            foreach (v; fi.get!(string[])("version"))
                c.addVersion(v);
            foreach (includePath; fi.get!(string[])("includePath"))
                c.addIncludePath(includePath);
            c.setObjectFilepath(fileCache.filepath(fi));
            return c.execute(true);
        }
        fileCache.create(fi, &compile);

        //Collect the object filename
        objectFilepaths ~= fileCache.filepath(fi);
    }

    //Link the executable, if necessary
    string execName = getName(filepath);
    auto fi = FileInfo(execName);
    foreach (objectFilepath; objectFilepaths)
        fi.add("objectFilepath", objectFilepath);
    string uniqExecName = fileCache.filepath(fi);
    bool link(FileInfo fi)
    {
        auto l = new LinkExecutable(uniqExecName);
        foreach (objectFilepath; fi.get!(string[])("objectFilepath"))
            l.addObjectFilepath(objectFilepath);
        return l.execute(true);
    }
    fileCache.create(fi, &link);
    //Copy the executable over
    //Copying with the copy()-function does not preserve the executable attributes, so we use the shell instead
    system(Format.immediate("cp %s %s", uniqExecName, execName));
}

enum ExitCode
{
    ok,
    error
}
void exitApp(T...)(ExitCode exitCode, string formatString, T args)
{
    switch (exitCode)
    {
        case ExitCode.error: write("ERROR::"); break;
        default: break;
    }
    writefln(formatString, args);
    switch (exitCode)
    {
        case ExitCode.ok: c_process.exit(0); break;
        case ExitCode.error: c_process.exit(-1); break;
    }
}
