#ifndef HEADER_da_package_Arduino_hpp_ALREADY_INCLUDED
#define HEADER_da_package_Arduino_hpp_ALREADY_INCLUDED

#include "da/package/Package.hpp"
#include "gubg/file/File.hpp"

namespace da
{
    namespace package
    {
        class Arduino: public Package
        {
            public:
                template <typename T>
                static Ptr create(T t){return Ptr(new Arduino(t));}

                //Package API
                virtual std::string name() const {return "arduino";}
                virtual bool exists() const;
                virtual ReturnCode resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial);

            private:
                Arduino(const gubg::file::File &base);

                gubg::file::File base_;
        };
    }
}

#endif
