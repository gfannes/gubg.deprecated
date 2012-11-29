#ifndef gubg_Verbose_hpp
#define gubg_Verbose_hpp

#include <iostream>

namespace gubg
{
    template <bool B>
        class Verbose
        {
            public:
                Verbose():
                    verbose_(B){}

                bool verbose() const {return verbose_;}
                template <typename Arg>
                    bool verbose(const Arg &arg)
                    {
                        if (verbose_)
                            std::cout << arg << std::endl;
                        return verbose();
                    }
                template <typename Arg1, typename Arg2, typename... Args>
                    bool verbose(const Arg1 &arg1, const Arg2 &arg2, const Args... args)
                    {
                        if (verbose_)
                            std::cout << arg1 << " ";
                        return verbose(arg2, args...);
                    }

            private:
                bool verbose_;
        };
}

#endif
