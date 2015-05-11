#ifndef HEADER_fff_Types_hpp_ALREADY_INCLUDED
#define HEADER_fff_Types_hpp_ALREADY_INCLUDED

#include "fff/TagValue.hpp"
#include <set>
#include <map>

namespace fff { 

    using Dependencies = std::set<TagValue>;
    using RecursiveDependencies = std::map<TagValue, Dependencies>;

} 

#endif
