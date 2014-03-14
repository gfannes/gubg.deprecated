#ifndef HEADER_gubg_range_String_hpp_ALREADY_INCLUDED
#define HEADER_gubg_range_String_hpp_ALREADY_INCLUDED

#include <string>

namespace gubg { namespace range { 

    class Range_string
    {
        public:
            Range_string(std::string &str): str_(str) {}

            typedef std::string::value_type value_type;

            bool empty() const {return size_ == 0;}

            value_type &front() {return str_[ix_];}
            const value_type &front() const {return str_[ix_];}

            void popFront() {++ix_; --size_;}

            //Resizing does not invalidate existing ranges
            bool resizeBack(std::size_t nr)
            {
                if (nr <= size_)
                {
                    //The actual range size is even big enough
                    size_ = nr;
                    return true;
                }

                const auto wantedStringSize = ix_ + nr;
                if (wantedStringSize > str_.size())
                    //The wanted size is bigger than what is actually present in the string: increase it
                    str_.resize(wantedStringSize);
                size_ = nr;
                return true;
            }

        private:
            std::string &str_;
            std::size_t ix_ = 0;
            std::size_t size_ = str_.size();
    };

    class Range_cstring
    {
        public:
            Range_cstring(const std::string &str): str_(str) {}

            typedef std::string::value_type value_type;

            bool empty() const {return size_ == 0;}

            const value_type &front() const {return str_[ix_];}

            void popFront() {++ix_; --size_;}

        private:
            const std::string &str_;
            std::size_t ix_ = 0;
            std::size_t size_ = str_.size();
    };

} } 

#endif
