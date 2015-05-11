#ifndef HEADER_fff_TagValue_hpp_ALREADY_INCLUDED
#define HEADER_fff_TagValue_hpp_ALREADY_INCLUDED

#include "fff/Tag.hpp"
#include "fff/Value.hpp"
#include <vector>
#include <ostream>

namespace fff { 

    struct TagValue
    {
        Tag tag;
        Value value;
        TagValue(const Tag &tag, const Value &value): tag(tag), value(value) {}

        bool operator<(const TagValue &rhs) const { return std::make_pair(tag, value) < std::make_pair(rhs.tag, rhs.value); }
    };
    inline std::ostream &operator<<(std::ostream &os, const TagValue &tv)
    {
        os << tv.tag << ':' << tv.value;
        return os;
    }

	typedef std::vector<TagValue> TagValues;
} 

#endif
