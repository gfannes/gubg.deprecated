#ifndef HEADER_gubg_array_hpp_ALREADY_INCLUDED
#define HEADER_gubg_array_hpp_ALREADY_INCLUDED

typedef decltype(sizeof(int)) size_t;

namespace gubg
{
    template <typename T, size_t Nr>
        class array
        {
            public:
                class iterator;
                class const_iterator;

                array();
                array(const array &rhs);
                array &operator=(const array &rhs);

                iterator begin();
                iterator end();
                const_iterator begin() const;
                const_iterator end() const;

                T *data();
                const T *data() const;

            private:
                T data_[Nr];
        };

    template <typename T, size_t Nr>
        class array<T, Nr>::iterator
        {
            public:
                bool operator!=(const iterator &rhs) const {return ptr_ != rhs.ptr_;}
                iterator &operator++() {++ptr_; return *this;}
                iterator operator++(int) {iterator ret(ptr_); ++ptr_; return ret;}
                T &operator*() {return *ptr_;}

            private:
                friend class array<T, Nr>;
                iterator(T *ptr): ptr_(ptr) {}
                T *ptr_;
        };
    template <typename T, size_t Nr>
        class array<T, Nr>::const_iterator
        {
            public:
                bool operator!=(const const_iterator &rhs) const {return ptr_ != rhs.ptr_;}
                const_iterator &operator++() {++ptr_; return *this;}
                const_iterator operator++(int) {const_iterator ret(ptr_); ++ptr_; return ret;}
                const T &operator*() {return *ptr_;}

            private:
                friend class array<T, Nr>;
                const_iterator(const T *ptr): ptr_(ptr) {}
                const T *ptr_;
        };
    template <typename T, size_t Nr>
        array<T, Nr>::array()
        {
            for (auto it = begin(); it != end(); ++it)
                *it = T();
        }
    template <typename T, size_t Nr>
        array<T, Nr>::array(const array &rhs)
        {
            const_iterator src = rhs.begin();
            for (auto it = begin(); it != end(); ++it)
                *it = *src++;
        }
    template <typename T, size_t Nr>
        array<T, Nr> &array<T, Nr>::operator=(const array &rhs)
        {
            if (this != &rhs)
            {
                const_iterator src = rhs.begin();
                for (auto it = begin(); it != end(); ++it)
                    *it = *src++;
            }
            return *this;
        }
    template <typename T, size_t Nr>
        typename array<T, Nr>::iterator array<T, Nr>::begin()
        {
            return iterator(data_);
        }
    template <typename T, size_t Nr>
        typename array<T, Nr>::iterator array<T, Nr>::end()
        {
            return iterator(data_+Nr);
        }
    template <typename T, size_t Nr>
        typename array<T, Nr>::const_iterator array<T, Nr>::begin() const
        {
            return const_iterator(data_);
        }
    template <typename T, size_t Nr>
        typename array<T, Nr>::const_iterator array<T, Nr>::end() const
        {
            return const_iterator(data_+Nr);
        }
    template <typename T, size_t Nr>
        T *array<T, Nr>::data()
        {
            return data_;
        }
    template <typename T, size_t Nr>
        const T *array<T, Nr>::data() const
        {
            return data_;
        }
}

#endif
