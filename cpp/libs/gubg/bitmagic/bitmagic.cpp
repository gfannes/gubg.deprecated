#include "bitmagic.hpp"

unsigned int gubg::bitmagic::reverseBits(unsigned int v)
{
    if (0 == v)
        return v;
    //Exchange bits pairwise
    v = ((v & 0x55555555) << 1)  | ((v & 0xaaaaaaaa) >> 1);
    //Exchange bits in groups of 2
    v = ((v & 0x33333333) << 2)  | ((v & 0xcccccccc) >> 2);
    //Exchange bits in groups of 4
    v = ((v & 0x0f0f0f0f) << 4)  | ((v & 0xf0f0f0f0) >> 4);
    //Exchange bits in groups of 8
    v = ((v & 0x00ff00ff) << 8)  | ((v & 0xff00ff00) >> 8);
    //Exchange bits in groups of 16
    v = ((v & 0x0000ffff) << 16) | ((v & 0xffff0000) >> 16);
    return v;
}
