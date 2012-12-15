#include "gubg/coding/d9/rle.hpp"
#include "gubg/Testing.hpp"
#include <iostream>
using namespace gubg::coding::d9;
using namespace gubg::coding::d9::rle;
using namespace gubg::testing;
using namespace std;
#define L(m) cout<<m<<endl

int main()
{
    TEST_TAG(RLE);
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
            TEST_EQ(p->first, p2.first);
            TEST_EQ(p->second, p2.second);
        }
    }
    {
        Bits bits;
        string coded;
        Ixs ixs;
        coded = bits.encode();
        L("bits: " << toHex(coded));
        TEST_OK(Bits::decode(ixs, coded));
        TEST_TRUE(ixs.empty());
        bits.add(true);
        coded = bits.encode();
        L("bits: " << toHex(coded));
        TEST_OK(Bits::decode(ixs, coded));
        TEST_EQ(1, ixs.size());
        TEST_EQ(Ixs({0}), ixs);
        bits.add(false);
        L("bits: " << toHex(bits.encode()));
        for (int i = 0; i < 100; ++i)
            bits.add(true);
        L("bits: " << toHex(bits.encode()));
        bits.clear();
        L("bits: " << toHex(bits.encode()));
    }
    return 0;
}