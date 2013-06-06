#ifndef HEADER_gubg_cortex_IColumn_hpp_ALREADY_INCLUDED
#define HEADER_gubg_cortex_IColumn_hpp_ALREADY_INCLUDED

#include "gubg/cortex/Types.hpp"

namespace gubg
{
    namespace cortex
    {
        class IColumn
        {
            public:
                virtual Value value(time::Offset, time::Span) = 0;
        };
    }
}

#endif
