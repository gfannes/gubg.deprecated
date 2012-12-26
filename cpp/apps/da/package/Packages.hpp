#ifndef HEADER_da_package_Packages_hpp_ALREADY_INCLUDED
#define HEADER_da_package_Packages_hpp_ALREADY_INCLUDED

#include "da/package/Package.hpp"
#include "da/package/Boost.hpp"
#include <list>

namespace da
{
    namespace package
    {
        class Packages
        {
            public:
                template <typename Pkg>
                Packages &operator<<(const Pkg &pkg)
                {
                    packages_.push_back(Package::Ptr(new Pkg(pkg)));
                    return *this;
                }

                //Removes unexisting packages, and selects the first one if duplicates are present
                void prune();

                size_t size() const {return packages_.size();}

            private:
                typedef std::list<Package::Ptr> Packages_;
                Packages_ packages_;
        };
    }
}

#endif
