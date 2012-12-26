#ifndef HEADER_da_package_Boost_hpp_ALREADY_INCLUDED
#define HEADER_da_package_Boost_hpp_ALREADY_INCLUDED

#include "da/package/Package.hpp"
#include "gubg/file/File.hpp"

namespace da
{
    namespace package
    {
        class Boost: public Package
        {
            public:
                Boost();
                Boost(const gubg::file::File &base);

                //Package API
                virtual std::string name() const {return "boost";}

            private:
                gubg::file::File base_;
        };
    }
}

#endif
