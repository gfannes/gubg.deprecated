module Collection;

public import gubg.FSTree;
import gubg.parsing.D;
import std.path;
import std.algorithm;
import std.array;
import std.stdio;

class DCollection
{
    this (string path)
    {
        creator_ = new DCreator;
        internalTree_ = createFSTreeFromPath(path, creator_);
    }

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

    void addExternalTree(string path)
    {
        //Check that the path is not the same as the internal or any external path
        if (path == internalTree_.path)
            return;
        foreach (externalTree; externalTrees_)
            if (path == externalTree.path)
                return;

        //The path seems not to be present yet, so we add it
        externalTrees_ ~= createFSTreeFromPath(path, creator_);
    }

    void prune(string filepath)
    {
        writeln("Starting with pruning");
        auto parser = new DParser;
        //Collect recursively all imported modules and the filepath where this module can be found
        string[string] fpPerModule;
        auto modulesToCheck = uniq(parser.parse(filepath).imports);
        while (!modulesToCheck.empty)
        {
            writeln(modulesToCheck);
            auto newModulesToCheck = appender!(string[])();
            foreach (ref DFile file; this)
            {
                writeln("");
                if (!file.isTagged)
                {
                    foreach (modName; modulesToCheck)
                    {
                        writeln(modName);
                        if (DParser.fileMatchesModule(file.path, modName))
                        {
                        }
                    }
                }
            }
            modulesToCheck = uniq(newModulesToCheck.data);
        }

        //Remove all files that are not referenced
        // * Create a hash-lookup with our initial filepath and all referenced files
        bool[string] referencedFilepaths = [filepath: true];
        foreach (mod, fp; fpPerModule)
            referencedFilepaths[fp] = true;
        foreach(ref DFile file; this)
        {
            if (file.path in referencedFilepaths)
                file.remove;
        }
        writeln("Pruning is done");
        //Not implemented yet
        assert(false);
    }

    private:
    class DFile: gubg.FSTree.File
    {
        this(string path, Folder folder = null)
        {
            super(path, folder);
        }

        bool isTagged;
    }
    class DCreator: ICreator
    {
            Folder createFolder(string path)
            {
                return new Folder(path);
            }
            DFile createFile(string path)
            {
                if ("d" == getExt(path))
                    return new DFile(path);
                return null;
            }
    }
    DCreator creator_;
    FSTree internalTree_;
    FSTree[] externalTrees_;
}
