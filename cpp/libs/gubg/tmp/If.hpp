#ifndef HEADER_gubg_tmp_If_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tmp_If_hpp_ALREADY_INCLUDED

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
