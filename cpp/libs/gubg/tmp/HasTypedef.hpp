#ifndef HEADER_gubg_tmp_HasTypedef_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tmp_HasTypedef_hpp_ALREADY_INCLUDED

#include "gubg/tmp/Types.hpp"

namespace gubg
{
    namespace tmp
    {
        template <typename T>
            class HasTypedef
            {
                private:
                    template <typename TT> static Small Test(typename TT::Name *);
                    template <typename TT> static Large Test(...);
                public:
                    enum: bool {Value = (sizeof(Test<T>(0)) == sizeof(Small))};
            };
#if 0
        template <typename T, typename U>
            struct HasTypedef
            {
                enum: bool {Value = false};
            };
        template <typename T>
            struct HasTypedef<T, typename T::Name>
            {
                enum: bool {Value = true};
            };
#endif
    }
}

#endif
