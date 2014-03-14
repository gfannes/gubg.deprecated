#ifndef HEADER_gubg_range_Array_hpp_ALREADY_INCLUDED
#define HEADER_gubg_range_Array_hpp_ALREADY_INCLUDED

#include <cstddef>

namespace gubg { namespace range { 

    template <typename Value>
        struct Array
        {
            Value *data;
            size_t size;
        };

} } 

#endif
