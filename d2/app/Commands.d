module Commands;

import Collection;
import Exit;
import Configuration;
import Runtime;
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

private string fileCachePath_ = "/tmp/gb";
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
            res = new ExeCommand(args, true);
            break;
        case "lib":
            res = new LibCommand(args);
            break;
        case "doc":
            res = new DocCommand(args);
            break;
        case "clean":
            res = new CleanCommand;
            break;
        case "config":
            res = new ConfigCommand;
            break;
        default:
            break;
    }

    return res;
}

class ArgsCommand: ICommand
{
    this(string[] args)
    {
        args_ = args.dup;
    }

    abstract bool execute();
    abstract string toString();

    private:
    string[] args_;
}

class TagsCommand: ArgsCommand
{
    this(string[] args)
    {
        super(args);
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
            foreach (gubg.FSTree.File file; new DCollection(path))
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
}
class ExeCommand: ArgsCommand
{
    this(string[] args, bool isUnitTest = false)
    {
        super(args);
        isUnitTest_ = isUnitTest;
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

        //Create the Collection of all files to compile
        // * The initial tree defined by the main source file
        auto collection = new DCollection(dirname(filepath));
        // * Extend this set with the externalTrees
        auto config = new Configuration("gb.json", dirname(filepath), true);
        string[] externalTrees;
        config.get("externalTrees", externalTrees);
        foreach(ref externalTree; externalTrees)
        {
            externalTree = expandTilde(externalTree);
            collection.addExternalTree(externalTree);
        }
        // * Prune unreferenced files and get the necessay include paths also
        string[] includePaths;
        collection.prune(filepath, includePaths);
        if (Runtime.verbose)
        {
            writeln("I will build these files");
            foreach (gubg.FSTree.File file; collection)
                writefln("\t%s", file.path);
        }

        //Compile the sources and collect the object filepaths
        string[] objectFilepaths;
        auto fileCache = new FileCache(fileCachePath_);
        uint nrCompilationErrors = 0;
        uint nrCompilations = 0;
        foreach (gubg.FSTree.File file; collection)
        {
            //The source file that needs to be compiled
            string sourceFilepath = file.path;
            writeln(sourceFilepath);

            //Collect all information about the source file
            auto fi = FileInfo(addExt(sourceFilepath, "o"));
            if (isUnitTest_ && filepath == sourceFilepath)
                fi.add("version", "UnitTest");
            foreach(includePath; includePaths)
            fi.add("includePath", includePath);
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
            switch (fileCache.create(fi, &compile))
            {
                case FileCache.Result.AlreadyPresent:
                case FileCache.Result.CreationOK:
                    break;
                case FileCache.Result.CreationFailed:
                    exitApp(ExitCode.error, Format.immediate("ERROR::Failed to compile %s", sourceFilepath));
                    ++nrCompilationErrors;
                    writefln("ERROR::Failed to compile %s", sourceFilepath);
                    break;
            }
            ++nrCompilations;

            //Collect the object filename
            objectFilepaths ~= fileCache.filepath(fi);
        }
        if (nrCompilationErrors > 0)
        {
            writefln("I encountered %d compilation errors (out of %d).", nrCompilationErrors, nrCompilations);
            return false;
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
            string[] libraries;
            if (config.get("libraries", libraries))
            {
                foreach (library; libraries)
                    l.addLibrary(library);
            }
            return l.execute(true);
        }
        fileCache.create(fi, &link);
        //Copy the executable over
        //Copying with the copy()-function does not preserve the executable attributes, so we use the shell instead
        system(Format.immediate("cp %s %s", uniqExecName, execName));
        system(Format.immediate("cp %s.map %s.map", uniqExecName, execName));
        return true;
    }
    string toString(){return "ExeCommand";}

    private:
    bool isUnitTest_;
}
class LibCommand: ArgsCommand
{
    this(string[] args)
    {
        super(args);
    }
    //ICommand implementation
    bool execute(){return false;}
    string toString(){return "LibCommand";}
}
class DocCommand: ArgsCommand
{
    this(string[] args)
    {
        super(args);
    }
    //ICommand implementation
    bool execute(){return false;}
    string toString(){return "DocCommand";}
}
class CleanCommand: ICommand
{
    bool execute()
    {
        auto fc = new FileCache(fileCachePath_);
        fc.clean;
        return true;
    }
    string toString(){return "CleanCommand";}
}
class ConfigCommand: ICommand
{
    bool execute()
    {
        Format format;
        format.delimiter = "\n";
        format(`{`);
        format(`    "externalTrees": ["~/gubg/d2/gubg"]`);
        format(`    "libraries": ["dl", "cairo"]`);
        format(`}`);
        format(``);
        try
        {
            std.file.write("gb.json", format.toString);
        } catch(FileException)
        {
            return false;
        }
        return true;
    }
    string toString(){return "ConfigCommand";}
}
