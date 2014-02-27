#ifndef HEADER_gubg_Buffer_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Buffer_hpp_ALREADY_INCLUDED

#include <cstdint>
#include <cstddef>

namespace gubg { 

    class Range
    {
        public:
            std::uint8_t *front() const {return front_;}

            Range(std::uint8_t *b, size_t l): front_(b), size_(l) {}

        private:
            std::uint8_t *front_;
            size_t size_;
    };

    template <size_t Size_>
        class InputBuffer
        {
            public:
                const static size_t Size = Size_;
                typedef std::uint8_t Byte;

                InputBuffer(): end_(buffer_) {}

                void clear() { end_ = buffer_; }

                bool push_back(Byte b);

                Range sdu() const;

            private:
                Byte buffer_[Size];
                Byte *end_;
        };

    //Implementation
    template <size_t Size_>
        bool InputBuffer<Size_>::push_back(Byte b)
        {
            if (end_ - buffer_ >= Size)
                return false;
            *end_++ = b;
            return true;
        }
    template <size_t Size_>
        Range InputBuffer<Size_>::sdu() const
        {
            return Range(buffer_, end_ - buffer_);
        }


    template <size_t Size_>
        class OutputBuffer
        {
            public:
                const static size_t Size = Size_;

            private:
                typedef std::uint8_t Byte;
                Byte buffer_[Size];
        };

} 

#endif
