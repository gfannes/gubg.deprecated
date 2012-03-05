#include "gubg/coding/d9.hpp"
#include "gubg/testing/Testing.hpp"
#include <string>
#include <iostream>
using namespace gubg::coding::d9;
using namespace gubg::coding::d9;
using namespace gubg::testing;
using namespace std;
#define L(m) cout<<m<<endl

int main()
{
    TEST_TAG(d9);
    {
        TEST_TAG(RLE);
        using namespace rle;
        {
            auto numbers = {0, 1, 2, 63, 64, 65};
            for (auto number = numbers.begin(); number != numbers.end(); ++number)
                L(*number << " " << toHex(encodeNumber(*number)));

            for (unsigned long i = 0; i < 1024*1024; i += 13)
            {
                auto coded = encodeNumber(i);
                unsigned long v;
                decodeNumber(v, coded);
                TEST_EQ(i, v);
            }
        }
        {
            vector<pair<unsigned long, unsigned long>> pairs = {
                {0, 0}, {1, 10}, {2, 10}, {3, 10}, {4, 10}, {2, {20}}
            };
            for (auto p = pairs.begin(); p != pairs.end(); ++p)
            {
                auto coded = encodePair(p->first, p->second);
                L("Pair(" << p->first << ", " << p->second << "): " << toHex(coded) << "");
                TEST_EQ(coded, encodePair(*p));
                pair<unsigned long, unsigned long> p2;
                decodePair(p2, coded);
                TEST_EQ(p2.first, p->first);
                TEST_EQ(p2.second, p->second);
            }
        }
        {
            Bits bits;
            L("bits: " << toHex(bits.coded()));
            bits.add(true);
            L("bits: " << toHex(bits.coded()));
            bits.add(false);
            L("bits: " << toHex(bits.coded()));
            for (int i = 0; i < 100; ++i)
                bits.add(true);
            L("bits: " << toHex(bits.coded()));
            bits.clear();
            L("bits: " << toHex(bits.coded()));
        }
    }
#if 0
    {
        TEST_TAG(encode);
        vector<string> plains = {"", "abc", "a" "\xd8" "b" "\xd9" "c", "\xd8" "\xd9" "\xd8" "\xd9" "\xd8" "\xd9" "\xd8"};
        for (auto plain = plains.begin(); plain != plains.end(); ++plain)
        {
            L("plain : " << toHex(*plain));
            string coded;
            {
                coded = encode(*plain, Format::Block);
                L("block : " << toHex(coded));
            }
            {
                coded = encode(*plain, Format::Stream);
                L("stream: " << toHex(coded));
            }
            L("");
        }
    }
#endif
    {
        TEST_TAG(Package);
        string coded;
        auto formats = {Format::Block, Format::Stream};
        for (auto format = formats.begin(); format != formats.end(); ++format)
        {
            Package pkg;
            pkg.format(*format);
            pkg.encode(coded);
            L("coded(" << to_s(*format) << "): " << toHex(coded));
            vector<string> plains = {"", "abc", "a" "\xd8" "b" "\xd9" "c", "\xd8" "\xd9" "\xd8" "\xd9" "\xd8" "\xd9" "\xd8"};
            for (auto plain = plains.begin(); plain != plains.end(); ++plain)
            {
                pkg.content(*plain);
                L("plain: " << toHex(*plain));
                pkg.encode(coded);
                L("coded(" << to_s(*format) << "): " << toHex(coded));
            }
        }
    }
    return 0;
}
