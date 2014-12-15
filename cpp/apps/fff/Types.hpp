#ifndef HEADER_fff_Types_hpp_ALREADY_INCLUDED
#define HEADER_fff_Types_hpp_ALREADY_INCLUDED

#include "fff/TagValue.hpp"
#include "gubg/hash/MD5.hpp"
#include <string>
#include <set>

namespace fff { 

	typedef gubg::hash::md5::Hash Hash;
    typedef std::set<TagValue> Dependencies;

} 

#endif
