#ifndef HEADER_gubg_d9_Helper_hpp_ALREADY_INCLUDED
#define HEADER_gubg_d9_Helper_hpp_ALREADY_INCLUDED

#include "gubg/d9/Types.hpp"

namespace gubg
{
    namespace d9
    {
        template <typename String>
            size_t nrDXBytesInBuffer(const String &plain)
            {
                size_t nr = 0;
                auto end = plain.end();
                for (auto it = plain.begin(); it != end; ++it)
                {
                    const unsigned char byte = *it;
                    if (Byte::D8 == byte || Byte::D9 == byte)
                        ++nr;
                }
                return nr;
            }
    }
}

#endif
