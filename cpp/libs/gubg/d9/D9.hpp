#ifndef gubg_D9_hpp
#define gubg_D9_hpp

#include "gubg/d9/Helper.hpp"
#include <cassert>

namespace gubg
{
    namespace d9
    {
        template <typename Dst, typename Src>
            bool encode(Dst &dst, const Src &src)
            {
                //Compute the total size of encoding src
                const auto nrDx = nrDXBytesInBuffer(src);
                const size_t nrFlipBytes = 1+nrDx/7;
                const size_t length = 1+nrFlipBytes+src.size()+2;

                //Resize the destination buffer
                dst.resize(length);
                if (dst.size() != length)
                    //Resizing dst failed
                    return false;

                //Create the it iterator which will be used to add marker bytes and content
                typename Dst::iterator it = dst.begin();
                *it++ = Byte::D9;

                //Createt the flip iterator which will be used to keep track of 0xd9 into 0xd8 flipping
                typename Dst::iterator flip = it;
                *flip = 0x00;
                size_t nrFlips = 0;
                const size_t NrFlipsPerByte = 7;
                it += nrFlipBytes;

                //Add all bytes to the buffer using it
                //Translate 0xd9 into 0xd8 and keep track of the flips using flip
                for (ubyte ch: src)
                {
                    switch ((ubyte)ch)
                    {
                        case Byte::D9:
                            //Fallthrough is intentional
                            *flip |= (1 << nrFlips);
                            ch = Byte::D8;
                        case Byte::D8:
                            if (++nrFlips >= NrFlipsPerByte)
                            {
                                nrFlips = 0;
                                ++flip;
                            }
                            break;
                    }
                    *it++ = ch;
                }
                //Set the MSBit of the last flip
                assert((*flip & 0xc0) == 0x00);//The two MSBits should not be used
                *flip |= 0x80;
                //Add the closing marker
                *it++ = Byte::D9;
                *it++ = Byte::End;

                assert(it == dst.end());

                return true;
            }
    }
}

#endif
