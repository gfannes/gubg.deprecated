#include "gubg/coding/d9.hpp"
using namespace std;

#include <iostream>
#define L(m) cout<<m<<endl

//                --package-start--   alterations        ----------0xd9-free-buffer--------------- ---package-end---
//                0xd9     0xd9       |||||||   0xd8                                               0xd9     0xd8
// * Block mode:  11011001 11011001 (0???????)* 11011000 <0xd9-free data, same size as orig data>  11011001 11011000
// * Stream mode: 11011001 11011001  10000000            (<0xd8/9-free data> | 11011000 ###?????)* 11011001 11011000

namespace
{
    const unsigned char D8 = 0xd8;
    const unsigned char D9 = 0xd9;
    const unsigned char BlockByte =  0xff;
    const unsigned char StreamByte = 0x80;

    size_t nrD8D9Bytes(const string &plain)
    {
        size_t nr = 0;
        auto end = plain.end();
        for (auto it = plain.begin(); it != end; ++it)
        {
            const unsigned char byte = *it;
            if (D8 == byte || D9 == byte)
                ++nr;
        }
        return nr;
    }
    void encodeInBlockMode(string &coded, const string &plain)
    {
        const size_t nrAlterationBytes = (nrD8D9Bytes(plain)+6)/7;
        const size_t codedSize = 2 + nrAlterationBytes + 1 + plain.size() + 2;
        coded.resize(codedSize);

        //dst will iterate over coded and fill it immediately, except for the alterations
        string::iterator dst = coded.begin();

        //Package start
        *(dst++) = D9; *(dst++) = D9;

        int nrA;                  //Counts the number of alterations we have already stored in al
        string::iterator al = dst;//Iterates slowly over the alterations
        //Proceed dst over the alteration bytes and initialize them to 0
        for (size_t i = 0; i < nrAlterationBytes; ++i)
            *(dst++) = 0;

        //Marks the end of the alterations
        *(dst++) = BlockByte;

        //Copy plain into coded (using dst), changing D9 into D8, and updating the alterations if necessary
        auto end = plain.end();
        for (auto it = plain.begin(); it != end; ++it)
        {
            const unsigned char byte = *it;
            switch (byte)
            {
                case D9: *al |= (1 << nrA);
                    //Fall-through is intentional
                case D8: if (++nrA > 6)
                         {
                             ++al;
                             nrA = 0;
                         }
                         *(dst++) = D8;
                         break;
                default: *(dst++) = byte; break;
            }
        }

        //Package end
        *(dst++) = D9; *(dst++) = D8;
    }

    struct EscapeSequence
    {
        size_t nr;
        unsigned char alterations;

        EscapeSequence():
            nr(0), alterations(0){}

        void addD8(){++nr;}
        void addD9(){alterations |= (1 << (nr++));}
        void writeAndReset(ostream &os)
        {
            if (!nr)
                return;
            os << D8 << (unsigned char)((nr << 5) | alterations);
            nr = 0;
            alterations = 0;
        }
        bool isFull() const {return nr >= 5;}
    };
    void encodeInStreamMode(string &coded, const string &plain)
    {
        ostringstream oss;
        oss << D9 << D9 << StreamByte;
        auto end = plain.end();
        EscapeSequence es;
        for (auto it = plain.begin(); it != end; ++it)
        {
            const unsigned char byte = *it;
            switch (byte)
            {
                case D8: es.addD8(); break;
                case D9: es.addD9(); break;
                default:
                         es.writeAndReset(oss);
                         oss << byte;
                         break;
            }
            if (es.isFull())
                es.writeAndReset(oss);
        }
        es.writeAndReset(oss);
        oss << D9 << D8;
        coded = oss.str();
    }
}

namespace gubg
{
    namespace coding
    {
        namespace d9
        {
            string encode(const string &plain, Mode mode)
            {
                string coded;
                switch (mode)
                {
                    case Block:  encodeInBlockMode (coded, plain); break;
                    case Stream: encodeInStreamMode(coded, plain); break;
                }
                return std::move(coded);
            }

            ReturnCode decode(string &plain, const string &coded, Mode *mode)
            {
                MSS_BEGIN(ReturnCode);
                MSS_END();
            }
        }
    }
}
