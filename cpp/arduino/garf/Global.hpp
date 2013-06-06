#ifndef HEADER_garf_Global_hpp_ALREADY_INCLUDED
#define HEADER_garf_Global_hpp_ALREADY_INCLUDED

//Placement new does apparently not exist for Arduino
typedef decltype(sizeof(int)) size_t;
void * operator new (size_t size, void * ptr) { return ptr; }

//For Arduino, you have to be carefull and not make Arduino lib calls in the ctor of a global variable.
namespace garf
{
    //Does a lazy initialization with placement new to make sure all Arduino
    //initialization is done
    template <typename T, int IX = 0>
        class Global
        {
            public:
                void init()
                {
                    if (!init_)
                        new (buffer_) T;
                    init_ = true;
                }

                T* operator->()
                {
                    init();
                    return reinterpret_cast<T*>(buffer_);
                }
                const T* operator->() const
                {
                    init();
                    return reinterpret_cast<const T*>(buffer_);
                }

            private:
                static char buffer_[sizeof(T)];
                static bool init_;
        };
    template <typename T, int IX>
        char Global<T, IX>::buffer_[sizeof(T)];
    template <typename T, int IX>
        bool Global<T, IX>::init_ = false;
}

#endif
