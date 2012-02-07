#ifndef gubg_pointer_StackPointer_hpp
#define gubg_pointer_StackPointer_hpp

namespace gubg
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
                bool isValid() const {return valid;}
                template <typename... Args>
                    bool create(Args&&... args)
                    {
                        if (valid)
                            return false;
                        new (buffer) T(std::forward<Args>(args)...);
                        valid = true;
                        return true;
                    }
                template <typename... Args>
                    T &operator()(Args&&... args)
                    {
						create(std::forward<Args>(args)...);
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
