#ifndef HEADER_gubg_coding_d9_rle_hpp_ALREADY_INCLUDED
#define HEADER_gubg_coding_d9_rle_hpp_ALREADY_INCLUDED

//Runlength encoding/decoding of:
// * Numbers
// * Bits
// * Pairs

#include "gubg/coding/d9/codes.hpp"
#include "gubg/coding/d9/types.hpp"
#include "gubg/coding/d9/io.hpp"
#include <string>
#include <istream>

namespace gubg
{
    namespace coding
    {
        namespace d9
        {
            namespace rle
            {
                ReturnCode readRLE(std::string &coded, IInput &);

                std::string     encodeNumber(unsigned long);
                ReturnCode decodeNumber(unsigned long &number, const std::string &coded);
                ReturnCode decodeNumber(unsigned long &number, IInput &);
                template <typename Number>
                    ReturnCode decodeNumber(Number &number, IInput &input)
                    {
                        MSS_BEGIN(ReturnCode);
                        unsigned long v;
                        MSS(decodeNumber(v, input));
                        number = v;
                        MSS_END();
                    }
                std::string     encodePair(unsigned long first, unsigned long second);
                ReturnCode decodePair(unsigned long &first, unsigned long &second, const std::string &coded);
                ReturnCode decodePair(unsigned long &first, unsigned long &second, IInput &);
                template <typename Base>
                    std::string encodePair(std::pair<Base, Base> p){return encodePair(p.first, p.second);}
                template <typename Base>
                    ReturnCode decodePair(std::pair<Base, Base> &p, const std::string &coded){return decodePair(p.first, p.second, coded);}

                class Bits
                {
                    public:
                        Bits();
                        void add(bool);
                        std::string encode() const;
                        //Returns the ixs where a 1 is found
                        static ReturnCode decode(Ixs &ixs, const std::string &coded);
                        static ReturnCode decode(Ixs &ixs, IInput &);
                        void clear();
                        bool empty() const;
                    private:
                        void appendCurrentToBuffer_();

                        ubyte current;
                        ubyte nr;
                        std::string buffer;
                };
            }
        }
    }
}

#endif
