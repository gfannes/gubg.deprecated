module Collection;

public import gubg.FSTree;
import gubg.parsing.D;
import gubg.parsing.Cpp;
import gubg.Format;

import std.path;
import std.algorithm;
import std.array;

import std.stdio;

bool verbose__ = false;

//A collection of filesystem trees containing D files
class Collection
{
    this (string path)
    {
        scope factory = new Factory(path);
        creator_ = factory.createCreator(path);
        
        internalTree_ = createFSTreeFromPath(path, creator_);
    }

    //Adds an extra filesystem tree
    void addExternalTree(string path)
    {
        //Check that the path is not the same as the internal or any external path
        if (path == internalTree_.path)
            return;
        foreach (externalTree; externalTrees_)
            if (path == externalTree.path)
                return;

        //The path seems not to be present yet, so we add it
        //We make sure that _none_ of the already present files will be part of this new external tree
        bool[string] allIncludedPaths;
        foreach (ref TaggedFile file; this)
            allIncludedPaths[file.path] = true;
        auto externalTree = createFSTreeFromPath(path, creator_);
        bool atLeastOneLeft = false;
        foreach (ref gubg.FSTree.File file; externalTree)
        {
            if (file.path in allIncludedPaths)
                file.remove;
            else
                atLeastOneLeft = true;
        }
        if (atLeastOneLeft)
            externalTrees_ ~= externalTree;
    }

    //Iterates over all filesystem trees
    int opApply(FileT)(int delegate(ref FileT) dg)
    {
        //We first iterate over the internal tree
        foreach (gubg.FSTree.File file; internalTree_)
        {
            FileT f = cast(FileT)file;
            if (f && dg(f)) break;
        }
        //Next, we iterate over the external trees
        foreach (externalTree; externalTrees_)
        {
            foreach (gubg.FSTree.File file; externalTree)
            {
                FileT f = cast(FileT)file;
                if (f && dg(f)) break;
            }
        }
        return 0;
    }

    //Prunes the collection of source files to just those files that can be recursively reached
    //from filepath. The required include paths are appended to includePaths, if they are not yet present
    void prune(string filepath, ref string[] includePaths)
    {
        if (verbose__)
        {
            writefln("Pruning include paths for %s", filepath);
            writefln("includePaths: %s", includePaths);
        }

        bool[string] isPathIncluded;

        {
            scope factory = new Factory(filepath);
            scope pruner = factory.createPruner;
            pruner.prune(filepath, includePaths, isPathIncluded);
        }

        //Remove all files that are not tagged (referenced)
        foreach(ref TaggedFile file; this)
        {
            if (!file.isTagged)
                file.remove;
        }

        //Append all extra include paths found
        foreach (includePath; includePaths)
        {
            if (includePath in isPathIncluded)
                isPathIncluded.remove(includePath);
        }
        includePaths ~= isPathIncluded.keys;
    }

    private:
    abstract class TaggedFile: gubg.FSTree.File
    {
        this(string path, Folder folder = null)
        {
            super(path, folder);
        }

        bool isTagged;
    }
    class DFile: TaggedFile
    {
        this(string path, Folder folder = null)
        {
            super(path, folder);
        }
    }
    class CppFile: TaggedFile
    {
        this(string path, Folder folder = null)
        {
            super(path, folder);
        }
    }
    class HppFile: TaggedFile
    {
        this(string path, Folder folder = null)
        {
            super(path, folder);
        }
    }
    abstract class BaseCreator: ICreator
    {
        this (string basepath)
        {
            basepath_ = basepath;
        }
        Folder createFolder(string path)
        {
                //We refuse to create test folders, except if this was our basepath, which is meant
                //for compiling unit tests
            if ("test" == basename(path) && basepath_ != path)
                return null;
            if (verbose__)
                writefln("createFolder: %s", path);
            return new Folder(path);
        }
        private:
        string basepath_;
    }
    class DCreator: BaseCreator
    {
        this (string basepath){super(basepath);}
        DFile createFile(string path)
        {
            if ("d" == getExt(path))
            {
                if (verbose__)
                    writefln("createFile: %s", path);
                return new DFile(path);
            }
            return null;
        }
    }
    class CppCreator: BaseCreator
    {
        this (string basepath){super(basepath);}
        TaggedFile createFile(string path)
        {
            auto extension = getExt(path);
            switch (extension)
            {
                case "cpp":
                    if (verbose__)
                        writefln("createFile: %s", path);
                    return new CppFile(path);
                    break;
                case "h":
                case "hpp":
                    if (verbose__)
                        writefln("createFile: %s", path);
                    return new HppFile(path);
                    break;
                default: break;
            }
            return null;
        }
    }
    class AllCreator: BaseCreator
    {
        this (string basepath){super(basepath);}
        TaggedFile createFile(string path)
        {
            auto extension = getExt(path);
            switch (extension)
            {
                case "cpp":
                    if (verbose__)
                        writefln("createFile: %s", path);
                    return new CppFile(path);
                    break;
                case "h":
                case "hpp":
                    if (verbose__)
                        writefln("createFile: %s", path);
                    return new HppFile(path);
                    break;
                case "d":
                    if (verbose__)
                        writefln("createFile: %s", path);
                    return new DFile(path);
                    break;
                default: break;
            }
            return null;
        }
    }
    class Factory
    {
        this (string filepath)
        {
            switch (getExt(filepath))
            {
                case "d": sourceType_ = SourceType.D; break;
                case "cpp": sourceType_ = SourceType.Cpp; break;
                default: break;
            }
        }
        ICreator createCreator(string basepath)
        {
            switch (sourceType_)
            {
                case SourceType.D: return new DCreator(basepath); break;
                case SourceType.Cpp: return new CppCreator(basepath); break;
                case SourceType.All: return new AllCreator(basepath); break;
            }
            assert(false);
            return null;
        }
        IPruner createPruner()
        {
            switch (sourceType_)
            {
                case SourceType.D: return new DPruner; break;
                case SourceType.Cpp: return new CppPruner; break;
                case SourceType.All: return new AllPruner; break;
            }
            assert(false);
            return null;
        }

        private:
        enum SourceType {D, Cpp, All};
        SourceType sourceType_ = SourceType.All;
    }
    class DPruner: IPruner
    {
        void prune(string filepath, ref string[] includePaths, ref bool[string] isPathIncluded)
        {
            auto parser = new DParser;
            //Collect recursively all imported modules and the filepath where this module can be found
            string[string] fpPerModule;
            auto modulesToCheck = parser.parse(filepath).imports;
            for (uint level = 0; !modulesToCheck.empty; ++level)
            {
                if (verbose__)
                {
                    writefln("Looking for imported modules level %d: %d modules to find.", level, modulesToCheck.length);
                    writefln("modulesToCheck: %s", modulesToCheck);
                }
                bool[string] newModulesToCheck;
                foreach (ref DFile file; this.outer)
                {
                    if (!file.isTagged)
                    {
                        if (filepath == file.path)
                        {
                            file.isTagged = true;
                        } else
                        {
                            foreach (modName; modulesToCheck)
                            {
                                auto includePath = DParser.includePathForModule(file.path, modName);
                                if (includePath !is null)
                                {
                                    if (verbose__)
                                        writefln("I found module %s in %s", modName, includePath);
                                    file.isTagged = true;
                                    foreach (newImport; uniq(parser.parse(file.path).imports))
                                        newModulesToCheck[newImport] = true;
                                    isPathIncluded[includePath] = true;
                                }
                            }
                        }
                    }
                }
                modulesToCheck = newModulesToCheck.keys;
            }
        }
    }
    class CppPruner: IPruner
    {
        void prune(string filepath, ref string[] includePaths, ref bool[string] isPathIncluded)
        {
            auto parser = new CppParser;
            //Collect recursively all included headers and sources and the filepath where these can be found
            string[string] fpPerModule;
            auto headersToCheck = parser.parse(filepath).includes;
            for (uint level = 0; !headersToCheck.empty; ++level)
            {
                if (verbose__)
                {
                    writefln("Looking for included headers level %d: %d headers to find.", level, headersToCheck.length);
                    writefln("headersToCheck: %s", headersToCheck);
                }
                bool[string] newHeadersToCheck;
                //Find the header files from headersToCheck and collecting source files to look for
                string[] sourcePaths;
                foreach (ref HppFile file; this.outer)
                {
                    if (!file.isTagged)
                    {
                        foreach (headerName; headersToCheck)
                        {
                            auto includePath = CppParser.includePathForHeader(file.path, headerName);
                            if (includePath !is null)
                            {
                                if (verbose__)
                                    writefln("I found header %s in %s", headerName, includePath);
                                file.isTagged = true;
                                sourcePaths ~= std.path.addExt(file.path, "cpp");
                                foreach (newInclude; uniq(parser.parse(file.path).includes))
                                    newHeadersToCheck[newInclude] = true;
                                isPathIncluded[includePath] = true;
                            }
                        }
                    }
                }
                //Check if there are source files corresponding to these header files
                foreach (ref CppFile file; this.outer)
                {
                    if (!file.isTagged)
                    {
                        if (filepath == file.path)
                        {
                            file.isTagged = true;
                        } else
                        {
                            foreach (sourcePath; sourcePaths)
                            {
                                if (sourcePath == file.path)
                                {
                                    if (verbose__)
                                        writefln("I found source %s", sourcePath);
                                    file.isTagged = true;
                                    foreach (newInclude; uniq(parser.parse(file.path).includes))
                                        newHeadersToCheck[newInclude] = true;
                                }
                            }
                        }
                    }
                }
                headersToCheck = newHeadersToCheck.keys;
            }
            //Untag all header files, as these need no compilation
            foreach (ref HppFile file; this.outer)
                file.isTagged = false;
        }
    }
    class AllPruner: IPruner
    {
        void prune(string filepath, ref string[] includePaths, ref bool[string] isPathIncluded)
        {
            throw new Exception("Not yet implemented");
        }
    }
    ICreator creator_;
    FSTree internalTree_;
    FSTree[] externalTrees_;
}

interface IPruner
{
    void prune(string filepath, ref string[] includePaths, ref bool[string] isPathIncluded);
}
