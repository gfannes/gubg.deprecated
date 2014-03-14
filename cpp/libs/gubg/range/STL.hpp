#ifndef HEADER_gubg_range_STL_hpp_ALREADY_INCLUDED
#define HEADER_gubg_range_STL_hpp_ALREADY_INCLUDED

#include <string>
#include <vector>

namespace gubg { namespace range { 

    template <typename CharT, typename TraitsT, typename AllocatorT>
        class Range_string
        {
            public:
                typedef std::basic_string<CharT, TraitsT, AllocatorT> String;

                Range_string(String &str): str_(str), pos_(0), size_(str_.size()) {}

                String &str() {return str_;}
                const String &str() const {return str_;}

                //range API
                typedef CharT value_type;

                bool empty() const;
                value_type &front() const;

            private:
                String &str_;
                std::size_t pos_;
                std::size_t size_;
        };

    template <typename CharT, typename TraitsT, typename AllocatorT>
        class Range_cstring
        {
            public:
                typedef std::basic_string<CharT, TraitsT, AllocatorT> String;

                Range_cstring(String &str): str_(str), pos_(0), size_(str_.size()) {}

                String &str() {return str_;}
                const String &str() const {return str_;}

                //range API
                typedef const CharT value_type;

                bool empty() const;
                value_type &front() const;

            private:
                String &str_;
                std::size_t pos_;
                std::size_t size_;
        };

    template <typename Container>
        struct Traits { };
    template <>
        struct Traits<std::string>
        {
            typedef Range_string<char, std::char_traits<char>, std::allocator<char>> Range;
        };
    template <>
        struct Traits<const std::string>
        {
            typedef Range_cstring<char, std::char_traits<char>, std::allocator<char>> Range;
        };

    //Implementation
#define L_TEMPLATE template <typename CharT, typename TraitsT, typename AllocatorT>
#define L_SCOPE Range_string<CharT, TraitsT, AllocatorT>
    L_TEMPLATE
        bool L_SCOPE::empty() const { return size_ > 0; }
    L_TEMPLATE
        CharT &L_SCOPE::front() const { return str_[pos_]; }
#undef L_SCOPE

#define L_SCOPE Range_cstring<CharT, TraitsT, AllocatorT>
    L_TEMPLATE
        bool L_SCOPE::empty() const { return size_ > 0; }
    L_TEMPLATE
        const CharT &L_SCOPE::front() const { return str_[pos_]; }
#undef L_SCOPE
#undef L_TEMPLATE

} } 

#endif
