#ifndef HEADER_gubg_range_String_hpp_ALREADY_INCLUDED
#define HEADER_gubg_range_String_hpp_ALREADY_INCLUDED

#include <string>

namespace gubg { namespace range { 

    class String_char
    {
        public:
            String_char(std::string &str): str_(&str) {}

            typedef std::string::value_type value_type;

            bool empty() const {return size_ == 0;}

            value_type &front() {return (*str_)[ix_];}
            const value_type &front() const {return (*str_)[ix_];}

            value_type &back() {return (*str_)[ix_+size_-1];}
            const value_type &back() const {return (*str_)[ix_+size_-1];}

            void popFront() {++ix_; --size_;}

            void popBack() {--size_;}

            //Resizing does not invalidate existing ranges
			//Returns true if the current range could be adjusted to have size == nr
			//The front() location will remain the same
            bool resizeBack(std::size_t nr)
            {
                if (nr <= size_)
                {
                    //The actual range size is even big enough
                    size_ = nr;
                    return true;
                }

                const auto wantedStringSize = ix_ + nr;
                if (wantedStringSize > str_->size())
                    //The wanted size is bigger than what is actually present in the string: increase it
					//We can do this without changing other ranges on this same str_
                    str_->resize(wantedStringSize);
                size_ = nr;
                return true;
            }
			//The back() location will remain the same
			bool resizeFront(std::size_t nr)
			{
                if (nr <= size_)
                {
                    //The actual range size is even big enough
					ix_ += size_ - nr;
                    size_ = nr;
                    return true;
                }

                const auto wantedSizeDelta = nr - size_;
				if (wantedSizeDelta > ix_)
				{
					//We cannot provide the wanted size by moving ix_ to front,
					//and we cannot resize str_ at the front since that would change
					//other ranges on this same str_
					return false;
				}
				ix_ -= wantedSizeDelta;
                size_ = nr;
                return true;
			}

        private:
            std::string *str_;
			//We use an index iso an iterator since the former
			//allows us to change str_, e.g., resizing it
            std::size_t ix_ = 0;
            std::size_t size_ = str_->size();
    };

    class String_cchar
    {
        public:
            //Make sure str outlives this
            String_cchar(const std::string &str): str_(&str) {}

            typedef std::string::value_type value_type;

            bool empty() const {return size_ == 0;}

            const value_type &front() const {return (*str_)[ix_];}
            const value_type &back() const {return (*str_)[ix_+size_-1];}

            void popFront() {++ix_; --size_;}

            void popBack() {--size_;}

        private:
            const std::string *str_;
            std::size_t ix_ = 0;
            std::size_t size_ = str_->size();
    };

} } 

#endif
