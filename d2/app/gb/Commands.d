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
import std.string;

private bool verbose__ = true;

interface ICommand
{
    bool execute();
    string toString();
}

version (Posix)
    private string fileCachePath_ = "/tmp/gb";
version (Windows)
    private string fileCachePath_ = "c:\\tmp\\gb";
ICommand[] createCommands(ref string[] args)
{
    ICommand[] commands;

    while (!args.empty)
    {
        auto commandString = args.front;
        if (verbose__)
            writefln("Command: %s", commandString);
        auto argsRestore = args;
        args.popFront;
        ICommand command;
        switch (commandString)
        {
            case "tags":
                command = new TagsCommand(args);
                break;
            case "exe":
                command = new ExeCommand(args);
                break;
            case "unit":
                command = new ExeCommand(args, true);
                break;
            case "moc":
                //Qt MetaObject Compiler
                command = new MocCommand(args);
                break;
            case "lib":
                command = new LibCommand(args);
                break;
            case "doc":
                command = new DocCommand(args);
                break;
            case "clean":
                command = new CleanCommand;
                break;
            case "config":
                command = new ConfigCommand;
                break;
            default:
                writefln("Unexpected command %s", commandString);
                args = argsRestore;
                return [];
                break;
        }
        commands ~= command;
    }

    return commands;
}

class ArgsCommand: ICommand
{
    enum Amount {Zero, One, All}
    this(ref string[] args, Amount amount)
    {
        final switch (amount)
        {
            case Amount.Zero:
                break;
            case Amount.One:
                if (args.empty())
                    throw new Exception("I expected one argument");
                args_ = [args.front()];
                args.popFront();
                break;
            case Amount.All:
                for (; !args.empty(); args.popFront())
                    args_ ~= args.front();
                break;
        }
    }

    abstract bool execute();
    abstract string toString();

    private:
    string[] args_;
}

class TagsCommand: ArgsCommand
{
    this(ref string[] args)
    {
        super(args, ArgsCommand.Amount.All);
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
            foreach (gubg.FSTree.File file; new Collection(path))
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
    this(ref string[] args, bool isUnitTest = false)
    {
        super(args, ArgsCommand.Amount.One);
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
        auto collection = new Collection(dirname(filepath));
        // * Extend this set with the location where the configuration file is found and any externalTrees that are defined
        auto config = new Configuration(Runtime.configurationFilename, dirname(filepath), true);
        if (config.isValid())
        {
            collection.addExternalTree(dirname(config.filepath));
            string[] externalTrees;
            config.get(externalTrees, "externalTrees");
            foreach(ref externalTree; externalTrees)
            {
                externalTree = expandTilde(externalTree);
                collection.addExternalTree(externalTree);
            }
        }
        // * Prune unreferenced files and get the necessay include paths also
        string[] includePaths;
        collection.prune(filepath, includePaths);
        if (Runtime.verbose)
        {
            writeln("I will build these files:");
            uint nr = 0;
            foreach (gubg.FSTree.File file; collection)
            {
                ++nr;
                writefln("\t%s", file.path);
            }
            writefln("\t => %s files", nr);
        }

        //Compile the sources and collect the object filepaths
        string[] objectFilepaths;
        auto fileCache = new FileCache(fileCachePath_);
        uint nrCompilationErrors = 0;
        uint nrCompilations = 0;
        // * Collect the compilation options, if any
        string[] compilationOptions;
        config.get(compilationOptions, "compilation");
        foreach (gubg.FSTree.File file; collection)
        {
            //The source file that needs to be compiled
            string sourceFilepath = file.path;
            if (Runtime.verbose)
                writefln("Processing %s", sourceFilepath);

            //Collect all information about the source file
            auto fi = FileInfo(addExt(sourceFilepath, gubg.Build.objectExtension()));
            if (isUnitTest_ && filepath == sourceFilepath)
                fi.add("version", "UnitTest");
            foreach(includePath; includePaths)
                fi.add("includePath", includePath);
            foreach(option; compilationOptions)
                fi.add("compilationOptions", option);
            fi.addFile(sourceFilepath);

            //Compile, if necessary
            bool compile(FileInfo fi)
            {
                auto c = new Compile(sourceFilepath);
                foreach (v; fi.get!(string[])("version"))
                    c.addVersion(v);
                foreach (includePath; fi.get!(string[])("includePath"))
                    c.addIncludePath(includePath);
                foreach (option; fi.get!(string[])("compilationOptions"))
                    c.addOption(option);
                c.setObjectFilepath(fileCache.filepath(fi));
                return c.execute(true);
            }
            final switch (fileCache.create(fi, &compile))
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
        string execName = createExecName_(getName(filepath));
        auto fi = FileInfo(execName);
        foreach (objectFilepath; objectFilepaths)
            fi.add("objectFilepath", objectFilepath);
        string[] linkingOptions;
        config.get(linkingOptions, "linking");
        foreach(option; linkingOptions)
            fi.add("linkingOptions", option);
        string uniqExecName = createExecName_(fileCache.filepath(fi));
        bool link(FileInfo fi)
        {
            auto l = new LinkExecutable(uniqExecName, gubg.Build.guessSourceType(filepath));
            foreach (objectFilepath; fi.get!(string[])("objectFilepath"))
                l.addObjectFilepath(objectFilepath);
            string[] libraries;
            if (config.get(libraries, "libraries"))
            {
                foreach (library; libraries)
                    l.addLibrary(library);
            }
            foreach (option; fi.get!(string[])("linkingOptions"))
                l.addOption(option);
            return l.execute(true);
        }
        fileCache.create(fi, &link);
        //Copy the executable over
        //Copying with the copy()-function does not preserve the executable attributes, so we use the shell instead
        system(Format.immediate("cp \"%s\" \"%s\"", uniqExecName, execName));
        version (Posix) system(Format.immediate("cp %s.map %s.map", uniqExecName, execName));
        return true;
    }
    string toString(){return "ExeCommand";}

    private:
    string createExecName_(string name) const
    {
        version (Posix)
        {
            if (isUnitTest_)
                return name.addExt("unit");
        }
        version (Windows)
        {
            return name.addExt("exe");
        }
        return name;
    }
    bool isUnitTest_;
}
class MocCommand: ArgsCommand
{
    this(ref string[] args)
    {
        super(args, ArgsCommand.Amount.Zero);
    }
    bool execute()
    {
        auto fileCache = new FileCache(fileCachePath_);
        auto collection = new Collection(std.path.curdir);
        foreach (gubg.FSTree.File file; collection)
        {
            auto filepath = file.path;
            if (filepath.getExt == "hpp")
            {
                auto mocFilepath = std.path.getName(filepath) ~ "_moc.cpp";
                auto fi = FileInfo(mocFilepath);
                fi.set("filepath", filepath);
                fi.addFile(filepath);

                //Moc, if necessary
                bool moc(FileInfo fi)
                {
                    auto filepath = fi.get!(string)("filepath");
                    auto content = cast(string)std.file.read(filepath);
                    if (std.string.indexOf(content, "Q_OBJECT") >= 0)
                    {
                        if (Runtime.verbose)
                            writefln("Running moc against %s => %s", filepath, mocFilepath);
                        return 0 == system(Format.immediate("moc %s -o %s", filepath, fileCache.filepath(fi)));
                    }
                    //We create an empty file ...
                    std.file.write(fileCache.filepath(fi), "");
                    return true;
                }
                final switch (fileCache.create(fi, &moc))
                {
                    case FileCache.Result.AlreadyPresent:
                    case FileCache.Result.CreationOK:
                        auto fp = fileCache.filepath(fi);
                        auto fs = std.file.getSize(fp);
                        if (fs > 0)
                        {
                            if (!std.file.exists(mocFilepath) || fs != std.file.getSize(mocFilepath))
                                system(Format.immediate("cp %s %s", fp, mocFilepath));
                        }
                        break;
                    case FileCache.Result.CreationFailed:
                        exitApp(ExitCode.error, Format.immediate("ERROR::Failed to moc %s", filepath));
                        writefln("ERROR::Failed to moc %s", filepath);
                        break;
                }
            }
        }
        return true;
    }
    string toString(){return "MocCommand";}
}
class LibCommand: ArgsCommand
{
    this(ref string[] args)
    {
        super(args, ArgsCommand.Amount.All);
    }
    //ICommand implementation
    bool execute(){return false;}
    string toString(){return "LibCommand";}
}
class DocCommand: ArgsCommand
{
    this(ref string[] args)
    {
        super(args, ArgsCommand.Amount.All);
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
            try
            {
                std.file.write(Runtime.configurationFilename(), Configuration.Configuration.example());
            } catch(FileException)
            {
                return false;
            }
            return true;
    }
    string toString(){return "ConfigCommand";}
}
