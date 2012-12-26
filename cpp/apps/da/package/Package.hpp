#ifndef HEADER_da_package_Package_hpp_ALREADY_INCLUDED
#define HEADER_da_package_Package_hpp_ALREADY_INCLUDED

#include <string>
#include <memory>

namespace da
{
    namespace package
    {
        class Package
        {
            public:
                typedef std::shared_ptr<Package> Ptr;

                virtual std::string name() const = 0;
                virtual bool exists() const = 0;
        };
    }
}

#endif
