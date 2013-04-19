#ifndef HEADER_da_Types_hpp_ALREADY_INCLUDED
#define HEADER_da_Types_hpp_ALREADY_INCLUDED

#include "gubg/OrderedSet.hpp"
#include "gubg/file/File.hpp"

namespace da
{
    typedef gubg::OrderedSet<gubg::file::File> IncludePaths;
    typedef gubg::OrderedSet<std::string> Defines;
    typedef gubg::OrderedSet<gubg::file::File> LibraryPaths;
    typedef gubg::OrderedSet<std::string> Libraries;
    typedef gubg::OrderedSet<std::string> CompileOptions;
    typedef gubg::OrderedSet<std::string> LinkOptions;
    typedef gubg::OrderedSet<gubg::file::File> SourceFiles;

    enum Platform {Unknown, Any, Host, Arduino};
    inline void merge(Platform &dst, const Platform &src)
    {
        if (dst == src)
            return;
        switch (dst)
        {
            case Any: dst = src; break;
            default: if (src != Any) dst = Unknown; break;
        }
    }

    enum class ExeType {Debug, Release};

    struct CompileSettings
    {
        IncludePaths includePaths;
        Defines defines;
        CompileOptions compileOptions;
        Platform targetPlatform;

        CompileSettings():
            targetPlatform(Any){}

        void clear()
        {
            includePaths.clear();
            defines.clear();
            compileOptions.clear();
            targetPlatform = Any;
        }
        void insert(const CompileSettings &cs)
        {
            includePaths.insert(cs.includePaths);
            defines.insert(cs.defines);
            compileOptions.insert(cs.compileOptions);
            merge(targetPlatform, cs.targetPlatform);
        }
    };

    struct LinkSettings
    {
        LibraryPaths libraryPaths;
        Libraries libraries;
        LinkOptions linkOptions;
        Platform targetPlatform;

        LinkSettings():
            targetPlatform(Any){}

        void clear()
        {
            libraryPaths.clear();
            libraries.clear();
            linkOptions.clear();
            targetPlatform = Any;
        }
        void insert(const LinkSettings &ls)
        {
            libraryPaths.insert(ls.libraryPaths);
            libraries.insert(ls.libraries);
            linkOptions.insert(ls.linkOptions);
            merge(targetPlatform, ls.targetPlatform);
        }
    };
}

#endif
