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
                //Compute the total size of encoding src
                const auto nrDx = nrDXBytesInBuffer(src);
                const size_t nrFlipBytes = 1+nrDx/7;
                const size_t length = 1+nrFlipBytes+src.size()+2;

                //Resize the destination buffer
                dst.resize(length);
                MSS(dst.size() == length, ResizeFailed);

                //Create the it iterator which will be used to add marker bytes and content
                typename Frame::iterator it = dst.begin();
                *it++ = Byte::D9;

                //Createt the flip iterator which will be used to keep track of 0xd9 into 0xd8 flipping
                typename Frame::iterator flip = it;
                *flip = 0x00;
                size_t flipOffset = 0;
                it += nrFlipBytes;

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
                //Set the MSBit of the last flip
                assert((*flip & 0xc0) == 0x00);//The two MSBits should not be used
                *flip |= 0x80;
                //Add the closing marker
                *it++ = Byte::D9;
                *it++ = Byte::End;

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

                typename Frame::const_iterator flip = src;
                typename Frame::const_iterator lastOfTheFlips;
                for (;;)
                {
                    lastOfTheFlips = src;
                    const ubyte b = *src++;
                    MSS(src != end, PlainTooShort);
                    if ((b & 0xc0) == 0x80)
                        break;
                }
                size_t flipOffset = 0;

                bool readingContent = true;
                while (readingContent)
                {
                    ubyte b = *src++;
                    MSS(src != end, PlainTooShort);
                    switch (b)
                    {
                        case Byte::D9:
                            readingContent = false;
                            break;
                        case Byte::D8:
                            //Make sure we don't read more that 6 flips from lastOfTheFlips
                            MSS(flip != lastOfTheFlips || flipOffset < 6, IllegalEncoding);
                            //Flip if necessary
                            if (*flip & (1 << flipOffset++))
                                b = Byte::D9;
                            //Proceed flip when exhausted
                            if (flipOffset >= NrFlipsPerByte)
                            {
                                ++flip;
                                flipOffset = 0;
                            }
                            dst.push_back(b);
                            break;
                        default:
                            dst.push_back(b);
                            break;
                    }
                }

                MSS(Byte::End == (ubyte)*src++, EndMarkerMissing);
                MSS(src == end, PlainTooLong);

                MSS_END();
            }
    }
}

#endif
