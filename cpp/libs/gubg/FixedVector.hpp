#ifndef gubg_FixedVector_hpp
#define gubg_FixedVector_hpp

//This module provides part of the std::vector API without relying on new and delete to be available (think e.g., arduino)
// * size()
// * iterators
// * push_back
// * clear
// * operator[]

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

                iterator begin() {return elements_;}
                const_iterator begin() const {return elements_;}
                iterator end() {return elements_ + Capacity;}
                const_iterator end() const {return elements_ + Capacity;}

                void push_back(T t)
                {
                    if (size_ >= Capacity)
                        return;
                    elements_[size_++] = t;
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

                T &operator[](size_t ix) {return elements_[ix];}

            private:
                T elements_[Capacity];
                size_t size_;
        };
}

#endif
