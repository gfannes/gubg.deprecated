module gb;

import Collection;
import gubg.Build;
import gubg.FileCache;
import gubg.Format;
import gubg.OnlyOnce;
import gubg.OptionParser;
import std.stdio;
import std.array;
import std.getopt;
import std.path;
import std.file;
import std.process;
import c_process = std.c.process;

immutable int versionMajor_ = 0;
immutable int versionMinor_ = 0;
immutable int versionBuild_ = 0;

//gb [options] <command> <location(s)>
//Options:
// * -v/--verbose
// * -h/--help
// * -V/--version
// * -o/--output <filename>
//Commands:
// * tags
// * exe
// * unit
// * lib
// * doc
//Location(s):
// * Sourcefile
// * "gb.json" file
// * Directory where to look for a gb.json file
int main(string[] args)
{
    //We will collect all command-line options in these two hashes
    bool[string] switches;
    string[string] values;

    //The OptionParser we will use to parse the command-line arguments
    auto parser = new OptionParser("Usage: gb [switches/options] <command> <location(s)>");

    //Options:
    parser.addLine("\nOptions:");
    // -h/--help
    parser.addSwitch("-h", "--help", "Print this help", (){exitApp(ExitCode.ok, parser.help);}); 
    // -v/--verbose
    switches["verbose"] = false;
    parser.addSwitch("-v", "--verbose", "Enable extra output", (){switches["verbose"] = true;});
    // -V/--version
    parser.addSwitch("-V", "--version", "Print the version", (){exitApp(ExitCode.ok, "Version %s.%s.%s", versionMajor_, versionMinor_, versionBuild_);});

    //Commands
    parser.addLine("\nCommands:");
    parser.addLine("\ttags\tCreate a ctags file");
    parser.addLine("\texe\tCreate an executable");
    parser.addLine("\tunit\tCreate and run a unit test");
    parser.addLine("\tlib\tCreate a library");
    parser.addLine("\tdoc\tCreate documentation");

    parser.addLine("\nWritten by Geert Fannes.");

    //Parse args
    if (!parser.parse(args))
        exitApp(ExitCode.error, Format.immediate("Failed to parse option \"%s\"\n\n%s", args.front, parser.help));

    bool createTagsOperation_()
    {
        string[] locations = [getcwd()];
        foreach (p; args)
            locations ~= rel2abs(p);
        writeln("I will collect tags for the following locations:");
        foreach (p; locations)
            writeln("\t"~p);

        OnlyOnce createTagsFile;
        foreach (path; locations)
        {
            foreach (gubg.Tree.File file; new DCollection(path))
            {
                writeln(file.path);
                string command;
                if (createTagsFile.firstTime)
                    command = Format.immediate("ctags %s", file.path);
                else
                    command = Format.immediate("ctags -a %s", file.path);
                writeln(command);
                system(command);
            }
        }

        return true;
    }
    bool createExecutableOperation_()
    {
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
        return true;
    }

    //The main operation to be executed
    bool delegate() operation_ = &createExecutableOperation_; 
    void createTags_(){operation_ = &createTagsOperation_;}

    //Parse command line options
    try
    {
        getopt(args,
                "tags", &createTags_);
    } catch (Exception)
    {
    }
    //Unshift the executable
    args = args[1 .. $];

    if (!operation_())
    {
        writeln("An error was encountered!");
        return 1;
    }
    writeln("Everything went OK");
    return 0;
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
