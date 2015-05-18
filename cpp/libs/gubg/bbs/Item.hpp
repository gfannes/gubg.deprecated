#ifndef HEADER_gubg_bbs_Item_hpp_ALREADY_INCLUDED
#define HEADER_gubg_bbs_Item_hpp_ALREADY_INCLUDED

#include <string>
#include <ostream>
#include <vector>

namespace gubg { namespace bbs {

    using Tag = std::string;
    using Value = std::string;

    using Tags = std::set<Tag>;

    class Item
    {
        public:
            Tag tag;
            Value value;

            Item() = default;
            Item(const Tag &t, const Value &v): tag(t), value(v) {}

            bool operator<(const Item &rhs) const
            {
                return std::make_pair(tag, value) < std::make_pair(rhs.tag, rhs.value);
            }
            bool operator==(const Item &rhs) const
            {
                return std::make_pair(tag, value) == std::make_pair(rhs.tag, rhs.value);
            }

        private:
    };
    inline std::ostream &operator<<(std::ostream &os, const Item &v)
    {
        return os << v.tag << ':' << v.value;
    }

	using Items = std::vector<Item>;

} } 

#endif
