#ifndef HEADER_da_package_Package_hpp_ALREADY_INCLUDED
#define HEADER_da_package_Package_hpp_ALREADY_INCLUDED

#include <string>

namespace da
{
    namespace package
    {
        class Package
        {
            public:
                virtual std::string name() const = 0;
        };
    }
}

#endif
