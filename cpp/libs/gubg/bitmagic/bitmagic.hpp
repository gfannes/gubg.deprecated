#ifndef HEADER_gubg_bitmagic_bitmagic_hpp_ALREADY_INCLUDED
#define HEADER_gubg_bitmagic_bitmagic_hpp_ALREADY_INCLUDED

#include <cstdint>
#include <cassert>

namespace gubg
{
    namespace bitmagic
    {
        unsigned int reverseBits(unsigned int v);

        inline uint32_t rotateLeft(uint32_t v, uint32_t count)
        {
            assert(count <= 32);
            return (v << count) | (v >> (32-count));
        }
        inline uint32_t rotateRight(uint32_t v, uint32_t count)
        {
            assert(count <= 32);
            return (v >> count) | (v << (32-count));
        }
    }
}

#endif
