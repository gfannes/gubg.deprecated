#ifndef HEADER_da_package_Package_hpp_ALREADY_INCLUDED
#define HEADER_da_package_Package_hpp_ALREADY_INCLUDED

#include "da/Types.hpp"
#include "da/Codes.hpp"
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

                void clearCompileSettings(){compileSettings_.clear();}
                void clearLinkSettings(){linkSettings_.clear();}

                void extractCompileSettings(CompileSettings &cs) const {cs.insert(compileSettings_);}
                void extractLinkSettings(LinkSettings &ls) const {ls.insert(linkSettings_);}

                virtual std::string name() const = 0;
                virtual bool exists() const = 0;

                //Searches the packages for a header
                virtual ReturnCode resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial) {return ReturnCode::UnknownHeader;}

                virtual void appendIncludePaths(IncludePaths &) const {};
                virtual void appendDefines(Defines &) const {};
                virtual void appendLibraryPaths(LibraryPaths &) const {};
                virtual void appendLibraries(Libraries &) const {};

            protected:
                virtual ~Package(){}

                CompileSettings compileSettings_;
                LinkSettings    linkSettings_;
        };
    }
}

#endif
