#ifndef gubg_D9_hpp
#define gubg_D9_hpp

#include "gubg/d9/Codes.hpp"
#include "gubg/d9/Helper.hpp"
#include <cassert>

namespace gubg
{
    namespace d9
    {
        template <typename Frame, typename Plain>
            ReturnCode encode(Frame &dst, const Plain &src)
            {
                MSS_BEGIN(ReturnCode);

                MSS(src.size() > 0, PlainTooShort);
                MSS(bool((src[0] & 0x80) == 0x80), FirstByteCannotBeFixPos);

                //Compute the total size of encoding src
                const auto nrDx = nrDXBytesInBuffer(src);
                const size_t nrFlipBytes = (nrDx+6)/7;
                const size_t length = 1+nrFlipBytes+src.size();

                //Resize the destination buffer
                dst.resize(length);
                MSS(dst.size() == length, ResizeFailed);

                //Create the it iterator which will be used to add marker bytes and content
                typename Frame::iterator it = dst.begin();
                *it++ = Byte::D9;

                //Create the flip iterator which will be used to keep track of 0xd9 into 0xd8 flipping
                //and set all the flip bytes to 0x00
                typename Frame::iterator flip = it;
                size_t flipOffset = 0;
                while (it != flip+nrFlipBytes)
                    *it++ = 0x00;

                //Add all bytes to the buffer using it
                //Translate 0xd9 into 0xd8 and keep track of the flips using flip
                for (ubyte ch: src)
                {
                    switch ((ubyte)ch)
                    {
                        case Byte::D9:
                            //Fallthrough is intentional
                            *flip |= (1 << flipOffset);
                            ch = Byte::D8;
                        case Byte::D8:
                            if (++flipOffset >= NrFlipsPerByte)
                            {
                                flipOffset = 0;
                                ++flip;
                            }
                            break;
                    }
                    *it++ = ch;
                }

                assert(it == dst.end());
                MSS_END();
            }

        template <typename Plain, typename Frame>
            ReturnCode decode(Plain &dst, const Frame &plain)
            {
                MSS_BEGIN(ReturnCode);

                typename Frame::const_iterator src = plain.begin();
                const typename Frame::const_iterator end = plain.end();

                MSS(Byte::D9 == (ubyte)*src++, StartMarkerExpected);
                MSS(src != end, PlainTooShort);

                //Initialize the flip iterator and proceed src to the first ubyte after the flips, if any
                typename Frame::const_iterator flip = src;
                for (;;)
                {
                    if ((*src & 0x80) == 0x80)
                        break;
                    ++src;
                    MSS(src != end, PlainTooShort);
                }
                const typename Frame::const_iterator flipEnd = src;
                size_t flipOffset = 0;

                while (src != end)
                {
                    ubyte b = *src++;
                    MSS(b != Byte::D9, IllegalEncoding);
                    if (b == Byte::D8)
                    {
                        MSS(flip != flipEnd, TooManyDx);
                        //Flip if necessary
                        if (*flip & (1 << flipOffset++))
                            b = Byte::D9;
                        //Proceed flip when exhausted
                        if (flipOffset >= NrFlipsPerByte)
                        {
                            ++flip;
                            flipOffset = 0;
                        }
                    }
                    dst.push_back(b);
                }

                MSS_END();
            }
    }
}

#endif
