#ifndef HEADER_gubg_Context_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Context_hpp_ALREADY_INCLUDED

#include <memory>

namespace gubg
{
    template <typename T>
        class Context
        {
            public:
                typedef std::shared_ptr<T> Ptr;
                Ptr context;

                Context(){}
                Context(const Context &c):context(c.context){}
                Context(const Ptr &ptr):context(ptr){}
                static Context create()
                {
                    Context c;
                    c.context.reset(new T);
                    return c;
                }

                void clear()
                {
                    context.reset();
                }

                Context &operator=(const Context &rhs)
                {
                    if (this == &rhs)
                        return *this;
                    context = rhs.context;
                    return *this;
                }
                Context &operator=(const Ptr &ptr)
                {
                    context = ptr;
                    return *this;
                }

                template <typename Rhs>
                    void set(const Rhs &rhs)
                    {
                        *this = rhs;
                    }
        };
}

#endif
