module Commands;

import Collection;
import Exit;
import gubg.OnlyOnce;
import gubg.Format;
import gubg.FileCache;
import gubg.Build;
import std.array;
import std.file;
import std.path;
import std.stdio;
import std.process;

interface ICommand
{
    bool execute();
    string toString();
}

ICommand createCommand(string[] args)
{
    ICommand res;

    if (args.empty)
        return res;

    auto commandString = args.front;
    args.popFront;
    switch (commandString)
    {
        case "tags":
            res = new TagsCommand(args);
            break;
        case "exe":
            res = new ExeCommand(args);
            break;
        case "unit":
            res = new UnitCommand(args);
            break;
        case "lib":
            res = new LibCommand(args);
            break;
        case "doc":
            res = new DocCommand(args);
            break;
        default:
            break;
    }

    return res;
}

class TagsCommand: ICommand
{
    this(string[] args)
    {
        args_ = args.dup;
    }
    //ICommand implementation
    bool execute()
    {
        string[] locations = [getcwd()];
        foreach (p; args_)
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
    string toString(){return "TagsCommand";}

    private:
    string[] args_;
}
class ExeCommand: ICommand
{
    this(string[] args)
    {
        args_ = args.dup;
    }
    //ICommand implementation
    bool execute()
    {
        //Get the filepath of the file to build
        string filepath;
        switch (args_.length)
        {
            case 0:
                exitApp(ExitCode.error, "You have to specify what file you want to build.");
                break;
            case 1:
                filepath = rel2abs(args_[0]);
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
    string toString(){return "ExeCommand";}

    private:
    string[] args_;
}
class UnitCommand: ICommand
{
    this(string[] args)
    {
    }
    //ICommand implementation
    bool execute(){return false;}
    string toString(){return "UnitCommand";}
}
class LibCommand: ICommand
{
    this(string[] args)
    {
    }
    //ICommand implementation
    bool execute(){return false;}
    string toString(){return "LibCommand";}
}
class DocCommand: ICommand
{
    this(string[] args)
    {
    }
    //ICommand implementation
    bool execute(){return false;}
    string toString(){return "DocCommand";}
}
