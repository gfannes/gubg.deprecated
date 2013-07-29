#ifndef HEADER_da_package_Libs_hpp_ALREADY_INCLUDED
#define HEADER_da_package_Libs_hpp_ALREADY_INCLUDED

#include "da/package/Package.hpp"
#include "gubg/file/File.hpp"

namespace da
{
    namespace package
    {
        class Libs: public Package
        {
            public:
                template <typename T>
                static Ptr create(T t){return Ptr(new Libs(t));}

                //Package API
                virtual std::string name() const {return "libs";}
                virtual bool exists() const;
                virtual ReturnCode resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial);

            private:
                Libs(const gubg::file::File &base);

                gubg::file::File base_;
                gubg::file::File libDir_;
        };
    }
}

#endif
