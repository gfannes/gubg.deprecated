#ifndef HEADER_fff_Types_hpp_ALREADY_INCLUDED
#define HEADER_fff_Types_hpp_ALREADY_INCLUDED

#include "fff/Tag.hpp"
#include "fff/Value.hpp"
#include "gubg/hash/MD5.hpp"
#include <string>

namespace fff { 

	typedef std::pair<Tag, Value> TagValue;
	typedef std::vector<TagValue> TagValues;
	typedef gubg::hash::md5::Hash Hash;
    typedef std::set<TagValue> Dependencies;

} 

#endif
