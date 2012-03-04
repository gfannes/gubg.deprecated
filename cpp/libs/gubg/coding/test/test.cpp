#include "gubg/coding/d9.hpp"
#include "gubg/testing/Testing.hpp"
#include <string>
#include <iostream>
using namespace gubg::coding::d9;
using namespace gubg::testing;
using namespace std;
#define L(m) cout<<m<<endl

int main()
{
    TEST_TAG(d9);
    {
        TEST_TAG(RLE);
        auto numbers = {0, 1, 2, 63, 64, 65};
        for (auto number = numbers.begin(); number != numbers.end(); ++number)
            L(*number << " " << toHex(encodeRLE(*number)));

        for (unsigned long i = 0; i < 1024*1024; i += 13)
        {
            auto rle = encodeRLE(i);
            unsigned long v;
            decodeRLE(v, rle);
            TEST_EQ(i, v);
        }
    }
    {
        TEST_TAG(encode);
        vector<string> plains = {"", "abc", "a" "\xd8" "b" "\xd9" "c", "\xd8" "\xd9" "\xd8" "\xd9" "\xd8" "\xd9" "\xd8"};
        for (auto plain = plains.begin(); plain != plains.end(); ++plain)
        {
            L("plain : " << toHex(*plain));
            string coded;
            {
                coded = encode(*plain, Block);
                L("block : " << toHex(coded));
            }
            {
                coded = encode(*plain, Stream);
                L("stream: " << toHex(coded));
            }
            L("");
        }
    }
    return 0;
}
