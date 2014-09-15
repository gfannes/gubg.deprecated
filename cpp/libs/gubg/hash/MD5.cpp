#include "gubg/hash/MD5.hpp"
#include "gubg/bitmagic/bitmagic.hpp"
#include "gubg/Platform.hpp"
#include <iomanip>
#include <sstream>
using namespace gubg::hash::md5;
using namespace std;

#ifndef GUBG_LITTLE_ENDIAN
#error This implementation is for little endian only. Interpreting messages as Word* will fail otherwise
#endif

//This implementation is based on the pseudocode provided by wikipedia
#if 0
//Note: All variables are unsigned 32 bit and wrap modulo 2^32 when calculating
var int[64] r, k

//r specifies the per-round shift amounts
r[ 0..15] := {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22}
r[16..31] := {5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20}
r[32..47] := {4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23}
r[48..63] := {6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21}

//Use binary integer part of the sines of integers (Radians) as constants:
for i from 0 to 63
    k[i] := floor(abs(sin(i + 1)) * (2 pow 32))
end for
//(Or just use the following table):
k[ 0.. 3] := { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee }
k[ 4.. 7] := { 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501 }
k[ 8..11] := { 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be }
k[12..15] := { 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 }
k[16..19] := { 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa }
k[20..23] := { 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8 }
k[24..27] := { 0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed }
k[28..31] := { 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a }
k[32..35] := { 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c }
k[36..39] := { 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70 }
k[40..43] := { 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05 }
k[44..47] := { 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 }
k[48..51] := { 0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039 }
k[52..55] := { 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1 }
k[56..59] := { 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1 }
k[60..63] := { 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 }

//Initialize variables:
var int h0 := 0x67452301   //A
var int h1 := 0xefcdab89   //B
var int h2 := 0x98badcfe   //C
var int h3 := 0x10325476   //D

//Pre-processing: adding a single 1 bit
append "1" bit to message    
// Notice: the input bytes are considered as bits strings,
  where the first bit is the most significant bit of the byte.[37]
  

//Pre-processing: padding with zeros
append "0" bit until message length in bit == 448 (mod 512)
append length mod (2 pow 64) to message


//Process the message in successive 512-bit chunks:
for each 512-bit chunk of message
    break chunk into sixteen 32-bit words w[j], 0 <= j <= 15
//Initialize hash value for this chunk:
    var int a := h0
    var int b := h1
    var int c := h2
    var int d := h3
//Main loop:
    for i from 0 to 63
        if 0 <= i <= 15 then
            f := (b and c) or ((not b) and d)
            g := i
        else if 16 <= i <= 31
            f := (d and b) or ((not d) and c)
            g := (5*i + 1) mod 16
        else if 32 <= i <= 47
            f := b xor c xor d
            g := (3*i + 5) mod 16
        else if 48 <= i <= 63
            f := c xor (b or (not d))
            g := (7*i) mod 16
        temp := d
        d := c
        c := b
        b := b + leftrotate((a + f + k[i] + w[g]) , r[i])
        a := temp
    end for
//Add this chunk's hash to result so far:
    h0 := h0 + a
    h1 := h1 + b
    h2 := h2 + c
    h3 := h3 + d
end for

var char digest[16] := h0 append h1 append h2 append h3 //(Output is in little-endian)

//leftrotate function definition
leftrotate (x, c)
    return (x << c) binary or (x >> (32-c));
#endif

namespace 
{
    typedef std::uint32_t Word;
    typedef std::array<Word, 4> HashWords;
    typedef HashWords State;
    typedef array<Word, 16> Constants;
    enum {A, B, C, D};//Do not change this lightly, the actual values are used as indices into State and HashWords

    string to_hex(const HashWords &hw)
    {
        std::ostringstream oss;
        oss << std::hex;
        for (size_t i = 0; i < 4; ++i)
            oss << "0x" << std::setfill('0') << std::setw(8) << hw[i] << " ";
        return oss.str();
    }
    string to_hex(const Word *words, size_t nr)
    {
        std::ostringstream oss;
        oss << std::hex;
        for (size_t i = 0; i < nr; ++i)
            oss << "0x" << std::setfill('0') << std::setw(8) << words[i] << " ";
        return oss.str();
    }

    enum RoundE {Zero, One, Two, Three};
    template <RoundE> struct Traits  {};
    template <> struct Traits<Zero>
    {
        static inline void nonLinearFunction(State &state) {state[A] += (state[B] & state[C]) | ((~state[B]) & state[D]);}
        static const Constants r;
        static const Constants k;
        static const Constants g;
    };
    const Constants Traits<Zero>::r = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22};
    const Constants Traits<Zero>::k = {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,};
    const Constants Traits<Zero>::g = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    template <> struct Traits<One>
    {
        static inline void nonLinearFunction(State &state) {state[A] += (state[B] & state[D]) | (state[C] & (~state[D]));}
        static const Constants r;
        static const Constants k;
        static const Constants g;
    };
    const Constants Traits<One>::r = {5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20};
    const Constants Traits<One>::k = {0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8, 0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,};
    const Constants Traits<One>::g = {1, 6, 11, 0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12};

    template <> struct Traits<Two>
    {
        static inline void nonLinearFunction(State &state) {state[A] += state[B] ^ state[C] ^ state[D];}
        static const Constants r;
        static const Constants k;
        static const Constants g;
    };
    const Constants Traits<Two>::r = {4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23};
    const Constants Traits<Two>::k = {0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,};
    const Constants Traits<Two>::g = {5, 8, 11, 14, 1, 4, 7, 10, 13, 0, 3, 6, 9, 12, 15, 2};

    template <> struct Traits<Three>
    {
        static inline void nonLinearFunction(State &state) {state[A] += state[C] ^ (state[B] | (~state[D]));}
        static const Constants r;
        static const Constants k;
        static const Constants g;
    };
    const Constants Traits<Three>::r = {6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};
    const Constants Traits<Three>::k = {0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };
    const Constants Traits<Three>::g = {0, 7, 14, 5, 12, 3, 10, 1, 8, 15, 6, 13, 4, 11, 2, 9};
}

#define GUBG_MODULE "BlockProcessor"
#include "gubg/log/begin.hpp"
namespace 
{
    struct BlockProcessor
    {
        HashWords &hash_;
        State state_;
        const Word *words_;

        BlockProcessor(HashWords &hash):
            hash_(hash), state_{}{}

        template <RoundE R>
            void round_()
            {
                S(); L(STREAM(to_hex(state_)));
                assert(words_);
                typedef Traits<R> Round;
                uint32_t cachedB;
                for (int oper = 0; oper < 16; ++oper)
                {
                    S(); L(STREAM(oper, to_hex(state_)));
                    Round::nonLinearFunction(state_);
                    state_[A] += words_[Round::g[oper]];
                    state_[A] += Round::k[oper];
                    cachedB = state_[B];
                    state_[B] += gubg::bitmagic::rotateLeft(state_[A], Round::r[oper]);
                    state_[A] = state_[D];
                    state_[D] = state_[C];
                    state_[C] = cachedB;
                }
                L(STREAM(to_hex(state_)));
            }

        void process(const Word *words)
        {
            S(); L(STREAM(to_hex(hash_)));

            assert(words);
            words_ = words;
            L(STREAM(to_hex(words_, 16)));

            state_ = hash_;

            round_<Zero>();
            round_<One>();
            round_<Two>();
            round_<Three>();

            hash_[A] += state_[A];
            hash_[B] += state_[B];
            hash_[C] += state_[C];
            hash_[D] += state_[D];

            L(STREAM(to_hex(hash_)));
        }
    };
}
#include "gubg/log/end.hpp"

Stream::Stream()
{
    clear();
}
void Stream::clear()
{
    hash_[0] = 0x67452301;
    hash_[1] = 0xefcdab89;
    hash_[2] = 0x98badcfe;
    hash_[3] = 0x10325476;
    length_ = 0;
    remainder_.clear();
}

Stream &Stream::operator<<(const string &message)
{
    const char *m = message.data();
    auto s = message.size();
    const auto rs = remainder_.size();

    if (s + rs < 64)
    {
        //The remainder and the new message combined do not fill a block, append and quit
        remainder_.append(m, s);
        return *this;
    }

    //We will at least process one block, time to install a block processor
    BlockProcessor bp(hash_);

    if (rs > 0)
    {
        //We have some left-over from the previous append, use this first.
        //We _do_ have a full block, complete remainder, process it and continue
        const auto c = 64-rs;
        remainder_.append(m, c);
        m += c;
        s -= c;

#ifndef GUBG_LITTLE_ENDIAN
#error Error
#endif
        bp.process((const Word *)remainder_.data());
        length_ += 512;
        //We do not clear remainder_, it will be assigned before leaving this method
    }

    //Process full blocks from the message only
    while (s >= 64)
    {
#ifndef GUBG_LITTLE_ENDIAN
#error Error
#endif
        bp.process((const Word *)m);
        length_ += 512;
        m += 64;
        s -= 64;
    }

    //Store the remainder that did not fit in a full block
    remainder_.assign(m, s);

    return *this;
}

Hash Stream::hash() const
{
    HashWords hash = hash_;

    BlockProcessor bp(hash);
    const auto rs = remainder_.size();
    const Length length = length_ + rs*8;
    if (rs < (64-8))
    {
        //There is enough room in the remainder block to include the 10000000|<length>-padding, so we just have to process one additional block
        auto block = remainder_;
        block.push_back((char)0x80);
        block.append(64-8-1-rs, '\0');
        //Make sure length is little endian
        assert(sizeof(length) == 8);
        block.append((char*)&length, 8);
        assert(block.size() == 64);
#ifndef GUBG_LITTLE_ENDIAN
#error Error
#endif
        bp.process((const Word *)block.data());
    }
    else
    {
        //There is not enough room in the remainder block to include the 10000000|<length>-padding, so we have to process two additional blocks
        auto block = remainder_;
        block.push_back((char)0x80);
        block.append(64-1-rs, '\0');
        assert(block.size() == 64);
#ifndef GUBG_LITTLE_ENDIAN
#error Error
#endif
        bp.process((const Word *)block.data());

        block.assign(56, '\0');
        //Make sure length is little endian
        assert(sizeof(length) == 8);
        block.append((char*)&length, 8);
        assert(block.size() == 64);
#ifndef GUBG_LITTLE_ENDIAN
#error Error
#endif
        bp.process((const Word *)block.data());
    }

    Hash ret;
    uint8_t *p = (uint8_t*)&hash[0];
    for (auto i = 0; i < 16; ++i)
        ret.raw[i] = *p++;

    return ret;
}
string Stream::hash_hex() const
{
    return hash().to_hex();
}

string Hash::to_hex() const
{
    std::ostringstream oss;
    oss << std::hex;
    for (auto i = 0; i < 16; ++i)
        oss << std::setfill('0') << std::setw(2) << (int)raw[i];
    return oss.str();
}
Hash &Hash::operator^=(const Hash &rhs)
{
	for (size_t i = 0; i < raw.size(); ++i)
		raw[i] ^= rhs.raw[i];
	return *this;
}
