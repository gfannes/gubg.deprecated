#ifndef HEADER_da_package_Local_hpp_ALREADY_INCLUDED
#define HEADER_da_package_Local_hpp_ALREADY_INCLUDED

#include "da/package/Package.hpp"
#include "gubg/file/File.hpp"
#include "gubg/file/Forest.hpp"

namespace da
{
    namespace package
    {
        class Local: public Package
        {
            public:
                template <typename T>
                static Ptr create(T t){return Ptr(new Local(t));}

                //Package API
                virtual std::string name() const;
                virtual bool exists() const;
                virtual ReturnCode resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial);

            private:
                Local(const gubg::file::File &base);

                gubg::file::File dir_;
                std::string basename_;
                gubg::file::Forest forest_;
        };
    }
}

#endif
