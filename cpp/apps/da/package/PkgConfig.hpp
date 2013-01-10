#ifndef da_package_PkgConfig_hpp
#define da_package_PkgConfig_hpp

#include "da/Codes.hpp"
#include "da/Types.hpp"
#include <vector>
#include <string>

namespace da
{
    namespace package
    {
        ReturnCode getIncludePathsForPackage(IncludePaths &, const std::string &pkg);
    }
}

#endif
