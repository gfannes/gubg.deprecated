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
                template <typename T>
                static Ptr create(T t){return Ptr(new GUBG(t));}

                //Package API
                virtual std::string name() const {return "gubg";}
                virtual bool exists() const;
                virtual ReturnCode resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial);

            private:
                GUBG(const gubg::file::File &base);

                gubg::file::File base_;
                gubg::file::File libsDir_;
                gubg::file::File appsDir_;
                gubg::file::Forest forest_;
        };
    }
}

#endif
