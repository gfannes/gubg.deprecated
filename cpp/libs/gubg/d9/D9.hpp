#ifndef HEADER_gubg_d9_D9_hpp_ALREADY_INCLUDED
#define HEADER_gubg_d9_D9_hpp_ALREADY_INCLUDED

#include "gubg/d9/Codes.hpp"
#include "gubg/d9/Helper.hpp"
#include "gubg/cassert.hpp"

#define GUBG_MODULE_ "d9"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace d9
    {
        template <typename Frame, typename Plain>
            ReturnCode encode(Frame &dst, const Plain &src)
            {
                MSS_BEGIN(ReturnCode);

                MSS(src.size() > 0, PlainTooShort);

                //Compute the total size of encoding src
                const auto nrDx = nrDXBytesInBuffer(src);
                const size_t nrFlipBytes = nrDx/7+1;
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
                const typename Frame::iterator last = flip+(nrFlipBytes-1);
                unsigned char flipOffset = 0;
                while (it != last)
                    *it++ = 0x00;
                *it++ = 0x80;//The last flip byte should start with 0b10******

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
                //The two MSBits of the last flip should still match 0b10******
                assert((*last & 0xc0) == 0x80);

                assert(it == dst.end());
                MSS_END();
            }

        template <typename Header, typename IO>
            ReturnCode encodeInPlace(Header &hdr, IO &io)
            {
                MSS_BEGIN(ReturnCode);

                MSS(io.size() > 0, PlainTooShort);

                //Compute the total size of encoding io
                const auto nrDx = nrDXBytesInBuffer(io);
                const size_t nrFlipBytes = nrDx/7+1;
                const size_t length = 1+nrFlipBytes;

                //Resize the d9 header buffer
                hdr.resize(length);
                MSS(hdr.size() == length, ResizeFailed);

                //Create the iterator which will be used to add marker bytes
                //and keep track of 0xd9 into 0xd8 flipping
                typename Header::iterator flip = hdr.begin();
                *flip++ = Byte::D9;

                const typename Header::iterator last = hdr.end()-1;
                while (flip != last)
                    *flip++ = 0x00;
                *flip++ = 0x80;//The last flip byte should start with 0b10******

                flip = hdr.begin()+1;
                unsigned char flipOffset = 0;

                //Add all bytes to the buffer using it
                //Translate 0xd9 into 0xd8 and keep track of the flips using flip
                for (ubyte &ch: io)
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
                }
                //The two MSBits of the last flip should still match 0b10******
                assert((*last & 0xc0) == 0x80);

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
                    if ((*src++ & 0xc0) == 0x80)
                        //This marks the end of the flips
                        break;
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
#include "gubg/log/end.hpp"

#endif
