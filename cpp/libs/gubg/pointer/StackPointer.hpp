#ifndef gubg_pointer_StackPointer_hpp
#define gubg_pointer_StackPointer_hpp

namespace
{
    template <typename T>
        class StackPointer
        {
            public:
                StackPointer():
                    valid(false){}
                ~StackPointer()
                {
                    if (valid)
                        reinterpret_cast<T*>(buffer)->~T();
                }
                template <typename... Args>
                    T &operator()(Args... args)
                    {
                        if (!valid)
                        {
                            new (buffer) T(args...);
                            valid = true;
                        }
                        return *reinterpret_cast<T*>(buffer);
                    }
            private:
                //We put the buffer first, else it might be badly aligned
                typedef unsigned char byte;
                byte buffer[sizeof(T)];
                bool valid;
        };
}

#endif
