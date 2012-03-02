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
    vector<string> plains;
    plains.push_back("");
    plains.push_back("abc");
    plains.push_back("a" "\xd8" "b" "\xd9" "c");
    plains.push_back("\xd8" "\xd9" "\xd8" "\xd9" "\xd8" "\xd9" "\xd8");

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
    return 0;
}
