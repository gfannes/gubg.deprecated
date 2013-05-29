#ifndef HEADER_da_package_Nana_hpp_ALREADY_INCLUDED
#define HEADER_da_package_Nana_hpp_ALREADY_INCLUDED

#include "da/package/Package.hpp"
#include "gubg/file/File.hpp"

namespace da
{
    namespace package
    {
        class Nana: public Package
        {
            public:
                template <typename T>
                static Ptr create(T t){return Ptr(new Nana(t));}

                //Package API
                virtual std::string name() const {return "nana";}
                virtual bool exists() const;
                virtual ReturnCode resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial);

            private:
                Nana(const gubg::file::File &base);

                gubg::file::File base_;
                gubg::file::File libDir_;
                gubg::file::File incDir_;
        };
    }
}

#endif
