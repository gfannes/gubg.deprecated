#ifndef HEADER_gubg_tmp_Equal_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tmp_Equal_hpp_ALREADY_INCLUDED

namespace gubg { namespace tmp {

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

} }

#endif
