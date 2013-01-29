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

    struct CompileSettings
    {
        IncludePaths includePaths;
        Defines defines;
        CompileOptions compileOptions;

        void clear()
        {
            includePaths.clear();
            defines.clear();
            compileOptions.clear();
        }
        void insert(const CompileSettings &cs)
        {
            includePaths.insert(cs.includePaths);
            defines.insert(cs.defines);
            compileOptions.insert(cs.compileOptions);
        }
    };

    struct LinkSettings
    {
        LibraryPaths libraryPaths;
        Libraries libraries;
        LinkOptions linkOptions;

        void clear()
        {
            libraryPaths.clear();
            libraries.clear();
            linkOptions.clear();
        }
        void insert(const LinkSettings &ls)
        {
            libraryPaths.insert(ls.libraryPaths);
            libraries.insert(ls.libraries);
            linkOptions.insert(ls.linkOptions);
        }
    };
}

#endif
