#ifndef HEADER_da_package_GUBG_hpp_ALREADY_INCLUDED
#define HEADER_da_package_GUBG_hpp_ALREADY_INCLUDED

#include "da/package/Package.hpp"
#include "gubg/file/File.hpp"
#include "gubg/file/Forest.hpp"

namespace da
{
    namespace package
    {
        class GUBG: public Package
        {
            public:
                GUBG();
                GUBG(const gubg::file::File &base);

                //Package API
                virtual std::string name() const {return "gubg";}
                virtual bool exists() const;
                virtual void appendIncludePaths(IncludePaths &) const;
                virtual void appendDefines(Defines &) const;
                virtual void appendLibraryPaths(LibraryPaths &) const;
                virtual void appendLibraries(Libraries &) const;
                virtual bool resolveHeader(File &resolvedHeader, File &includePath, SourceFiles &sisterFiles, const File &partial) const;

            private:
                gubg::file::File base_;
                gubg::file::File libsDir_;
                gubg::file::File appsDir_;
                gubg::file::Forest forest_;
        };
    }
}

#endif
