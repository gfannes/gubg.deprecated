#ifndef HEADER_da_package_UPP_hpp_ALREADY_INCLUDED
#define HEADER_da_package_UPP_hpp_ALREADY_INCLUDED

#include "da/package/Package.hpp"
#include "gubg/file/File.hpp"

namespace da
{
    namespace package
    {
        class UPP: public Package
        {
            public:
                UPP();
                UPP(const gubg::file::File &base);

                //Package API
                virtual std::string name() const {return "upp";}
                virtual bool exists() const;
                virtual void expandForest(Forest &) const;
                virtual void appendIncludePaths(IncludePaths &) const;

            private:
                gubg::file::File base_;
        };
    }
}

#endif
