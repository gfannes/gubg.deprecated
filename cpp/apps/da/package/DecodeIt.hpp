#ifndef HEADER_da_package_DecodeIt_hpp_ALREADY_INCLUDED
#define HEADER_da_package_DecodeIt_hpp_ALREADY_INCLUDED

#include "da/package/Package.hpp"
#include "gubg/file/File.hpp"
#include "gubg/file/Forest.hpp"

namespace da
{
    namespace package
    {
        class DecodeIt: public Package
        {
            public:
                template <typename T>
                static Ptr create(T t){return Ptr(new DecodeIt(t));}

                //Package API
                virtual std::string name() const {return "decode-it";}
                virtual bool exists() const;
                virtual ReturnCode resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial);

            private:
                DecodeIt(const gubg::file::File &base);

                gubg::file::File base_;
                gubg::file::File softDir_;
                gubg::file::Forest forest_;
        };
    }
}

#endif
