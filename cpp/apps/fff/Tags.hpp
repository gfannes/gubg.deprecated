#ifndef HEADER_fff_Tags_hpp_ALREADY_INCLUDED
#define HEADER_fff_Tags_hpp_ALREADY_INCLUDED

#include <string>
#include <set>

namespace fff { 

    class Tag
    {
        public:
            Tag(std::string tag, std::string descr):tag_(tag), descr_(descr)
        {
            tags_.insert(*this);
        }

            operator const std::string &() const {return tag_;}

            const std::string &tag() const {return tag_;}
            const std::string &descr() const {return descr_;}

            bool operator<(const Tag &rhs) const {return tag_ < rhs.tag_;}

            template <typename Callback>
                static void each_tag(Callback cb)
                {
                    for (auto &tag: tags_)
                        cb(tag);
                }

        private:
            const std::string tag_;
            const std::string descr_;
            using Tags = std::set<Tag>;
            static Tags tags_;
    };
    inline bool operator==(const std::string &lhs, const Tag &rhs) { return lhs == rhs.tag(); }
    inline bool operator!=(const std::string &lhs, const Tag &rhs) { return lhs != rhs.tag(); }

    extern const Tag cpp_define;
    extern const Tag cpp_header;
    extern const Tag cpp_source;
    extern const Tag cpp_source_ref;
    extern const Tag c_source;
    extern const Tag c_source_ref;
    extern const Tag hash_tag;

} 

#endif
