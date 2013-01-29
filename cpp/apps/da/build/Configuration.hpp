#ifndef HEADER_da_build_Configuration_hpp_ALREADY_INCLUDED
#define HEADER_da_build_Configuration_hpp_ALREADY_INCLUDED

#include "da/package/Packages.hpp"
#include "gubg/file/File.hpp"
#include "gubg/file/Forest.hpp"
#include <vector>
#include <string>

namespace da
{
    class Configuration
    {
        public:
            Configuration();

            da::package::Packages &packages() {return packages_;}
            const da::package::Packages &packages() const {return packages_;}

        private:
            da::package::Packages packages_;
    };
}

#endif
