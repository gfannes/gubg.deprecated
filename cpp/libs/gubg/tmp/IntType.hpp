#ifndef HEADER_gubg_tmp_IntType_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tmp_IntType_hpp_ALREADY_INCLUDED

namespace gubg { namespace tmp {

    template <int v>
        struct IntType
        {
            enum: int {value = v};
        };

} }

#endif
