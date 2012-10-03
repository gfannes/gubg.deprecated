#ifndef gubg_tmp_If_hpp
#define gubg_tmp_If_hpp

namespace gubg
{
    namespace tmp
    {
        template <bool Condition, typename Then, typename Else>
            struct If;
        template <typename Then, typename Else>
            struct If<true, Then, Else>
            {
                typedef Then Type;
            };
        template <typename Then, typename Else>
            struct If<false, Then, Else>
            {
                typedef Else Type;
            };
    }
}

#endif
