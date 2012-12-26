#ifndef HEADER_da_package_Package_hpp_ALREADY_INCLUDED
#define HEADER_da_package_Package_hpp_ALREADY_INCLUDED

#include "gubg/file/File.hpp"
#include "gubg/file/Forest.hpp"
#include <string>
#include <memory>
#include <vector>

namespace da
{
    namespace package
    {
        class Package
        {
            public:
                typedef std::shared_ptr<Package> Ptr;
                typedef std::vector<gubg::file::File> IncludePaths;
                typedef std::vector<gubg::file::File> LibraryPaths;
                typedef std::vector<std::string>      Libraries;
                typedef gubg::file::Forest            Forest;

                virtual std::string name() const = 0;
                virtual bool exists() const = 0;

                virtual void appendIncludePaths(IncludePaths &) const {};
                virtual void appendLibraryPaths(LibraryPaths &) const {};
                virtual void appendLibraries(Libraries &) const {};
                virtual void expandForest(Forest &) const {}
        };
    }
}

#endif
