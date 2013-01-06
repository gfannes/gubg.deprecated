#ifndef da_Types_hpp
#define da_Types_hpp

#include "gubg/OrderedSet.hpp"
#include "gubg/file/File.hpp"

namespace da
{
    typedef gubg::OrderedSet<gubg::file::File> IncludePaths;
    typedef gubg::OrderedSet<gubg::file::File> SourceFiles;
}

#endif
