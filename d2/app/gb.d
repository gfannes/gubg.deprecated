module gb;

import Collection;
import gubg.Compile;
import std.stdio;
import std.getopt;
import std.path;
import std.file;
import std.c.process;

immutable int versionMajor_ = 0;
immutable int versionMinor_ = 0;
immutable int versionBuild_ = 0;

void main(string[] args)
{
    //Short commands
    void printVersion()
    {
        writefln("Version %s.%s.%s", versionMajor_, versionMinor_, versionBuild_);
        exit(0);
    }
    void printHelp()
    {
        writeln(r"Generic build usage: gb [options] <file-to-build>
Options:
    --version       Prints the version
    --help          Prints this help

Created by Geert Fannes under the GPL.");
        exit(0);
    }

    //Parse command line options
    try
    {
    getopt(args,
            "version", &printVersion,
            "help", &printHelp);
    } catch (Exception)
    {
        printHelp();
    }
    //Unshift the executable
    args = args[1 .. $];

    //Get the filepath of the file to build
    string filepath;
    switch (args.length)
    {
        case 0:
            writeln("You have to specify what file you want to build.");
            exit(-1);
            break;
        case 1:
            filepath = rel2abs(args[0]);
            try
            {
                if (!isfile(filepath))
                {
                    writefln("File \"%s\" is not a file.", filepath);
                    exit(-1);
                }
            } catch (FileException)
            {
                writefln("File \"%s\" does not exist.", filepath);
                exit(-1);
            }
            break;
        default:
            writeln("You can only specify one file to build.");
            exit(-1);
            break;
    }
    writefln("I will build \"%s\".", filepath);

    //Create the Tree Collection
    auto collection = new DCollection(dirname(filepath));

    //Create the compilation command
    auto compileCommand = new CompileCommand(getName(filepath));
    foreach (gubg.Tree.File file; collection)
    {
        writeln(file.path);
        compileCommand.addSource(file.path);
    }

    //Execute the compilation command
    compileCommand.execute;
}
