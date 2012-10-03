#ifndef gubg_tmp_Equal_hpp
#define gubg_tmp_Equal_hpp

namespace gubg
{
    namespace tmp
    {
        template <typename T1, typename T2>
            struct Equal
            {
                enum: bool {Value = false};
            };
        template <typename T>
            struct Equal<T, T>
            {
                enum: bool {Value = true};
            };
    }
}

#endif
