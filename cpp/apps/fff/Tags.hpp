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

    extern const Tag cache_dir;
    extern const Tag cpp_define;
    extern const Tag cpp_executable;
    extern const Tag cpp_flag;
    extern const Tag cpp_force_include;
    extern const Tag cpp_header;
    extern const Tag cpp_include;
    extern const Tag cpp_include_path;
    extern const Tag cpp_library;
    extern const Tag cpp_library_path;
    extern const Tag cpp_object;
    extern const Tag cpp_shared_object;
    extern const Tag cpp_source;
    extern const Tag cpp_source_ref;
    extern const Tag cpp_tree;
    extern const Tag cpp_utree;
    extern const Tag c_header;
    extern const Tag c_object;
    extern const Tag c_source;
    extern const Tag c_source_ref;
    extern const Tag hash_tag;
    extern const Tag hash_md5sum;
    extern const Tag ig_ext;
    extern const Tag ig_fix;
    extern const Tag start;

} 

#endif
