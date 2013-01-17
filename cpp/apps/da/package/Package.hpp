#ifndef HEADER_da_package_Package_hpp_ALREADY_INCLUDED
#define HEADER_da_package_Package_hpp_ALREADY_INCLUDED

#include "da/Types.hpp"
#include "gubg/file/File.hpp"
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
                typedef gubg::file::File File;

                //Searches the packages for a header
                virtual bool resolveHeader(File &resolvedHeader, File &includePath, SourceFiles &sisterFiles, const File &partial) const {return false;}

                virtual std::string name() const = 0;
                virtual bool exists() const = 0;

                virtual void appendIncludePaths(IncludePaths &) const {};
                virtual void appendDefines(Defines &) const {};
                virtual void appendLibraryPaths(LibraryPaths &) const {};
                virtual void appendLibraries(Libraries &) const {};
        };
    }
}

#endif
