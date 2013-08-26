#ifndef HEADER_gubg_FixedVector_hpp_ALREADY_INCLUDED
#define HEADER_gubg_FixedVector_hpp_ALREADY_INCLUDED

//This module provides part of the std::vector API without relying on new and delete to be available (think e.g., arduino)
// * size()
// * iterators
// * push_back
// * clear
// * operator[]

#include "gubg/cassert.hpp"

namespace gubg
{
    template <typename T, size_t Capacity>
        class FixedVector
        {
            public:
                typedef T* iterator;
                typedef const T* const_iterator;

                FixedVector():size_(0){}

                size_t size() const {return size_;}
                bool empty() const {return size() == 0;}

                iterator begin() {return elements_;}
                const_iterator begin() const {return elements_;}
                iterator end() {return elements_ + size_;}
                const_iterator end() const {return elements_ + size_;}

                void push_back(T t)
                {
                    if (size_ >= Capacity)
                        return;
                    elements_[size_++] = t;
                }
                void pop_back()
                {
                    assert(!empty());
                    --size_;
                }
                void clear() {size_ = 0;}
                void resize(size_t wanted)
                {
                    if (wanted > Capacity)
                        wanted = Capacity;
                    while (size_ < wanted)
                        elements_[size_++] = T();
                    size_ = wanted;
                }

                T &operator[](size_t ix)
                {
                    assert(size_ > ix);
                    return elements_[ix];
                }
                T &back()
                {
                    assert(!empty());
                    return elements_[size_-1];
                }
                const T &operator[](size_t ix) const
                {
                    assert(size_ > ix);
                    return elements_[ix];
                }
                const T &back() const
                {
                    assert(!empty());
                    return elements_[size_-1];
                }

                const T *data() const
                {
                    assert(!empty());
                    return elements_;
                }

            private:
                T elements_[Capacity];
                size_t size_;
        };
}

#endif
