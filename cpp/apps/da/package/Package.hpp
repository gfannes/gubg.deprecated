#ifndef da_package_Package_hpp
#define da_package_Package_hpp

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
