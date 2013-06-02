#ifndef HEADER_gubg_Global_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Global_hpp_ALREADY_INCLUDED

namespace gubg
{
    template <typename T, int IX = 0>
        class Global
        {
            public:
                void init()
                {
                    new (buffer_) T;
                }

                T* operator->() {return reinterpret_cast<T*>(buffer_);}
                const T* operator->() const {return reinterpret_cast<const T*>(buffer_);}

            private:
                static char buffer_[sizeof(T)];
        };
    template <typename T, int IX>
        char Global<T, IX>::buffer_[sizeof(T)];
}

#endif
