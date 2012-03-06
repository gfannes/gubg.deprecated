#include "gubg/coding/d9/package.hpp"
#include "gubg/testing/Testing.hpp"
#include <iostream>
using namespace gubg::coding::d9;
using namespace gubg::testing;
using namespace std;
#define L(m) cout<<m<<endl

int main()
{
    TEST_TAG(Package);
    string coded;
    auto formats = {Format::Block, Format::Stream};
    for (auto format = formats.begin(); format != formats.end(); ++format)
    {
        auto checksums = {true, false};
        for (auto cs = checksums.begin(); cs != checksums.end(); ++cs)
        {
            Package pkg;
            pkg.format(*format);
            pkg.checksum(*cs);
            pkg.encode(coded);
            L("coded(" << to_s(*format) << ", " << (*cs ? "checksum" : "no checksum") << "): " << toHex(coded));
            vector<string> plains = {"", "abc", "a" "\xd8" "b" "\xd9" "c", "\xd8" "\xd9" "\xd8" "\xd9" "\xd8" "\xd9" "\xd8", string(100, '0'), string(100, 0xd9)};
            for (auto plain = plains.begin(); plain != plains.end(); ++plain)
            {
                pkg.content(*plain);
                L("plain: " << toHex(*plain));
                pkg.encode(coded);
                L("coded(" << to_s(*format) << "): length: " << coded.size() << ", hex: " << toHex(coded));
                {
                    Package pkg2;
                    TEST_OK(pkg2.decode(coded));
                    string plain2;
                    TEST_OK(pkg2.getContent(plain2));
                    TEST_EQ(*plain, plain2);
                }
            }
        }
    }
    return 0;
}
