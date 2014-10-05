#ifndef HEADER_enclosed_ptr_hpp_ALREADY_INCLUDED
#define HEADER_enclosed_ptr_hpp_ALREADY_INCLUDED

namespace gubg { 

    template <typename T, size_t Size>
        class enclosed_ptr
        {
            public:
                enclosed_ptr() = default;
                template <typename TT, size_t SS>
                    enclosed_ptr(const enclosed_ptr<TT, SS> &);
                template <typename TT, size_t SS>
                    enclosed_ptr &operator=(const enclosed_ptr<TT, SS> &);
                ~enclosed_ptr();

                T *get() throw();

                template <typename D>
                    T *create_new() throw();

                void reset();

            private:
                template <typename TT, size_t SS>
                    friend class enclosed_ptr;
                typedef void (*cctor_t)(void*, const void*);
                cctor_t cctor_ = nullptr;
                std::uint8_t data_[Size];
                size_t size_;
        };

    namespace details { 
        template <typename T>
            void cctor_(void *dst, const void *src)
            {
                //Placement-new copy constructor
                new ((void*)dst) T(*(const T*)src);
            }
    } 

    //Implementation
#define L_TEMPLATE template <typename T, size_t Size>
#define L_TYPE enclosed_ptr<T, Size>
    L_TEMPLATE
        L_TYPE::~enclosed_ptr()
        {
            if (auto ptr = get())
                ptr->~T();
        }
    L_TEMPLATE
        void L_TYPE::reset()
        {
            if (auto ptr = get())
                ptr->~T();
            cctor_ = 0;
        }
    L_TEMPLATE
        template <typename TT, size_t SS>
        L_TYPE::enclosed_ptr(const enclosed_ptr<TT, SS> &ep)
        {
            if (!ep.cctor_)
                return;
            if (Size < ep.size_)
                return;
            ep.cctor_(data_, ep.data_);
            size_ = ep.size_;
            cctor_ = ep.cctor_;
        }
    L_TEMPLATE
        template <typename TT, size_t SS>
        L_TYPE &L_TYPE::operator=(const enclosed_ptr<TT, SS> &rhs)
        {
            if (!rhs.cctor_)
                //rhs is invalid, we becomes invalid too
                reset();
            else if (Size < rhs.size_)
                //rhs is valid, but we have not enough place to store it
                reset();
            else
            {
                if (data_ != rhs.data_)
                {
                    rhs.cctor_(data_, rhs.data_);
                    size_ = rhs.size_;
                    cctor_ = rhs.cctor_;
                }
            }
            return *this;
        }
    L_TEMPLATE
        T *L_TYPE::get() throw()
        {
            if (!cctor_)
                return 0;
            return (T*)data_;
        }
    L_TEMPLATE
        template <typename D>
        T *L_TYPE::create_new() throw()
        {
            if (Size < sizeof(D))
                return 0;
            D *ptr = (D*)data_;
            //Placement-new constructor
            new ((void*)ptr) D();
            cctor_ = details::cctor_<D>;
            size_ = sizeof(D);
            return ptr;
        }
#undef L_TYPE
#undef L_TEMPLATE

} 

#endif
