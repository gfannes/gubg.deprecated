#include "gubg/coding/d9/rle.hpp"
using namespace std;

namespace 
{
    template <typename Source>
        string convertBytesToString_(Source &&bytes)
        {
            string res(bytes.size(), 0);
            auto end = bytes.end();
            size_t i = 0;
            for (auto it = bytes.begin(); it != end; ++it, ++i)
                res[i] = *it;
            return std::move(res);
        }
}

namespace gubg
{
    namespace coding
    {
        namespace d9
        {
            namespace rle
            {
                ReturnCode readRLE(string &coded, IInput &input)
                {
                    MSS_BEGIN(ReturnCode);
                    coded.clear();
                    while (true)
                    {
                        ubyte b;
                        MSS_T(input.read(b), RLETooSmall);
                        MSS_T((b&0xc0) != 0xc0, RLEIllegalMSBits);
                        coded.push_back(b);
                        if (b & 0x80)
                            break;
                    }
                    MSS_END();
                }
                string encodeNumber(unsigned long v)
                {
                    if (v <= 0x3f)
                        return convertBytesToString_({0x80 | v});
                    if (v <= 0x1fff)
                        return convertBytesToString_({(v&0x1fff)>>6, (v&0x3f)|0x80});
                    if (v <= 0xfffff)
                        return convertBytesToString_({(v&0xfffff)>>13, (v&0x1fff)>>6, (v&0x3f)|0x80});
                    if (v <= 0x7ffffff)
                        return convertBytesToString_({(v&0x7ffffff)>>20, (v&0xfffff)>>13, (v&0x1fff)>>6, (v&0x3f)|0x80});
                    return convertBytesToString_({v>>27, (v&0x7ffffff)>>20, (v&0xfffff)>>13, (v&0x1fff)>>6, (v&0x3f)|0x80});
                }
                ReturnCode decodeNumber(unsigned long &v, const string &coded)
                {
                    MSS_BEGIN(ReturnCode);
                    MSS_T(!coded.empty(), RLETooSmall);
                    MSS_T(coded.size() <= 5, RLETooLarge);
                    v = 0;
                    auto end = coded.end();
                    for (auto it = coded.begin(); it != end; ++it)
                    {
                        ubyte b = *it;
                        MSS_T((b&0xc0)!=0xc0, RLEIllegalMSBits);
                        if (b&0x80)
                        {
                            //This is the closing byte of the RLE
                            MSS_T(it+1 == end, RLEClosingByteExpected);
                            v <<= 6;
                            v |= (0x3f&b);
                        }
                        else
                        {
                            v <<= 7;
                            v |= (0x7f&b);
                        }
                    }
                    MSS_END();
                }
                ReturnCode decodeNumber(unsigned long &v, IInput &input)
                {
                    MSS_BEGIN(ReturnCode);
                    string coded;
                    MSS(readRLE(coded, input));
                    MSS(decodeNumber(v, coded));
                    MSS_END();
                }
                string encodePair(unsigned long first, unsigned long second)
                {
                    if (first <= 0x03 && second <= 0x0f)
                        return convertBytesToString_({0x80 | (0x30 & first<<4) | (0x0f & second)});
                    if (first <= 0x1f && second <= 0xff)
                        return convertBytesToString_({
                                (0x70 & (first>>2)<<4)   | (0x0f & second>>4),
                                0x80 | (0x30 & first<<4) | (0x0f & second)});
                    if (first <= 0xff && second <= 0xfff)
                        return convertBytesToString_({
                                (0x70 & (first>>5)<<4)   | (0x0f & second>>8),
                                (0x70 & (first>>2)<<4)   | (0x0f & second>>4),
                                0x80 | (0x30 & first<<4) | (0x0f & second)});
                    if (first <= 0x7ff && second <= 0xffff)
                        return convertBytesToString_({
                                (0x70 & (first>>8)<<4)   | (0x0f & second>>12),
                                (0x70 & (first>>5)<<4)   | (0x0f & second>>8),
                                (0x70 & (first>>2)<<4)   | (0x0f & second>>4),
                                0x80 | (0x30 & first<<4) | (0x0f & second)});
                    if (first <= 0x3fff && second <= 0xfffff)
                        return convertBytesToString_({
                                (0x70 & (first>>11)<<4)  | (0x0f & second>>16),
                                (0x70 & (first>>8)<<4)   | (0x0f & second>>12),
                                (0x70 & (first>>5)<<4)   | (0x0f & second>>8),
                                (0x70 & (first>>2)<<4)   | (0x0f & second>>4),
                                0x80 | (0x30 & first<<4) | (0x0f & second)});
                    if (first <= 0x1ffff && second <= 0xffffff)
                        return convertBytesToString_({
                                (0x70 & (first>>15)<<4)  | (0x0f & second>>20),
                                (0x70 & (first>>11)<<4)  | (0x0f & second>>16),
                                (0x70 & (first>>8)<<4)   | (0x0f & second>>12),
                                (0x70 & (first>>5)<<4)   | (0x0f & second>>8),
                                (0x70 & (first>>2)<<4)   | (0x0f & second>>4),
                                0x80 | (0x30 & first<<4) | (0x0f & second)});
                    if (first <= 0xfffff && second <= 0xfffffff)
                        return convertBytesToString_({
                                (0x70 & (first>>18)<<4)  | (0x0f & second>>24),
                                (0x70 & (first>>15)<<4)  | (0x0f & second>>20),
                                (0x70 & (first>>11)<<4)  | (0x0f & second>>16),
                                (0x70 & (first>>8)<<4)   | (0x0f & second>>12),
                                (0x70 & (first>>5)<<4)   | (0x0f & second>>8),
                                (0x70 & (first>>2)<<4)   | (0x0f & second>>4),
                                0x80 | (0x30 & first<<4) | (0x0f & second)});
                    if (first <= 0x7fffff && second <= 0xffffffff)
                        return convertBytesToString_({
                                (0x70 & (first>>21)<<4)  | (0x0f & second>>28),
                                (0x70 & (first>>18)<<4)  | (0x0f & second>>24),
                                (0x70 & (first>>15)<<4)  | (0x0f & second>>20),
                                (0x70 & (first>>11)<<4)  | (0x0f & second>>16),
                                (0x70 & (first>>8)<<4)   | (0x0f & second>>12),
                                (0x70 & (first>>5)<<4)   | (0x0f & second>>8),
                                (0x70 & (first>>2)<<4)   | (0x0f & second>>4),
                                0x80 | (0x30 & first<<4) | (0x0f & second)});
                    if (first <= 0x3ffffff && second <= 0xfffffffff)
                        return convertBytesToString_({
                                (0x70 & (first>>24)<<4)  | (0x0f & second>>32),
                                (0x70 & (first>>21)<<4)  | (0x0f & second>>28),
                                (0x70 & (first>>18)<<4)  | (0x0f & second>>24),
                                (0x70 & (first>>15)<<4)  | (0x0f & second>>20),
                                (0x70 & (first>>11)<<4)  | (0x0f & second>>16),
                                (0x70 & (first>>8)<<4)   | (0x0f & second>>12),
                                (0x70 & (first>>5)<<4)   | (0x0f & second>>8),
                                (0x70 & (first>>2)<<4)   | (0x0f & second>>4),
                                0x80 | (0x30 & first<<4) | (0x0f & second)});
                    return "you are probably abusing pair";
                }
                ReturnCode decodePair(unsigned long &first, unsigned long &second, const string &coded)
                {
                    MSS_BEGIN(ReturnCode);
                    MSS_T(!coded.empty(), RLETooSmall);
                    MSS_T(coded.size() <= 9, RLETooLarge);
                    first = second = 0;
                    auto end = coded.end();
                    for (auto it = coded.begin(); it != end; ++it)
                    {
                        ubyte b = *it;
                        MSS_T((b&0xc0)!=0xc0, RLEIllegalMSBits);
                        if (b&0x80)
                        {
                            //This is the closing byte of the RLE
                            MSS_T(it+1 == end, RLEClosingByteExpected);
                            first <<= 2;
                            first |= (0x03&b>>4);
                            second <<= 4;
                            second |= (0x0f&b);
                        }
                        else
                        {
                            first <<= 3;
                            first |= (0x07&b>>4);
                            second <<= 4;
                            second |= (0x0f&b);
                        }
                    }
                    MSS_END();
                }
                ReturnCode decodePair(unsigned long &first, unsigned long &second, IInput &input)
                {
                    MSS_BEGIN(ReturnCode);
                    string coded;
                    MSS(readRLE(coded, input));
                    MSS(decodePair(first, second, coded));
                    MSS_END();
                }

                //Bits
                Bits::Bits():
                    current(0), nr(0){}
                void Bits::add(bool b)
                {
                    if (b)
                        current |= (1 << nr);
                    const int nrBits = (buffer.empty() ? 6 : 7);
                    if (++nr >= nrBits)
                        appendCurrentToBuffer_();
                }
                string Bits::encode() const
                {
                    string res;
                    if (buffer.empty())
                    {
                        res.push_back(current | 0x80);
                    }
                    else
                    {
                        if (nr > 0)
                            res.push_back(current);
                        for (auto it = buffer.rbegin(); it != buffer.rend(); ++it)
                            res.push_back(*it);
                    }
                    return std::move(res);
                }
                ReturnCode Bits::decode(Ixs &ixs, const string &coded)
                {
                    MSS_BEGIN(ReturnCode);
                    ixs.clear();
                    unsigned long offset = 0;
                    auto end = coded.rend();
                    for (auto it = coded.rbegin(); it != end; ++it)
                    {
                        ubyte b = *it;
                        const int nrBits = (b & 0x80 ? 6 : 7);
                        for (int i = 0; i < nrBits; ++i)
                            if (b & 1<<i)
                                ixs.push_back(offset+i);
                        offset += nrBits;
                    }
                    MSS_END();
                }
                ReturnCode Bits::decode(Ixs &ixs, IInput &input)
                {
                    MSS_BEGIN(ReturnCode);
                    //Read a RLE encoded bits into a string
                    string coded;
                    {
                        while (true)
                        {
                            ubyte b;
                            MSS_T(input.read(b), UnexpectedBitsEnd);
                            MSS_T((b&0xc0) != 0xc0, RLEIllegalMSBits);
                            coded.push_back(b);
                            if (0x80 & b)
                                //This is the end-byte of the RLE
                                break;
                        }
                    }
                    //Convert the coded string into ixs
                    MSS(decode(ixs, coded));
                    MSS_END();
                }
                void Bits::clear()
                {
                    nr = current = 0;
                    buffer.clear();
                }
                bool Bits::empty() const
                {
                    return buffer.empty() && nr == 0;
                }
                void Bits::appendCurrentToBuffer_()
                {
                    if (buffer.empty())
                        current |= 0x80;
                    buffer.push_back(current);
                    nr = 0;
                    current = 0;
                }
            }
        }
    }
}
