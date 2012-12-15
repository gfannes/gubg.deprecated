#include "gubg/Testing.hpp"
#include "gubg/hash/MD5.hpp"
#include "gubg/l.hpp"
using namespace gubg::hash;

namespace 
{
    bool hash(MD5 &md5, const std::string &message, const std::string &expected)
    {
        md5.clear();
        md5 << message;
        const auto hash = md5.hash();
        const auto h1 = MD5::to_hex(hash);
        const auto h2 = md5.hash_hex();
        L("Hash: " << h1);
        return expected == h1 && h1 == h2;
    }
}

int main()
{
    TEST_TAG(main);
    MD5 md5;

    TEST_TRUE(hash(md5, "", "d41d8cd98f00b204e9800998ecf8427e"));
    TEST_TRUE(hash(md5, "The quick brown fox jumps over the lazy dog", "9e107d9d372bb6826bd81d3542a419d6"));
    TEST_TRUE(hash(md5, "The quick brown fox jumps over the lazy dog.", "e4d909c290d0fb1ca068ffaddf22cbd0"));
    TEST_TRUE(hash(md5, "aaa", "47bce5c74f589f4867dbd57e9ca9f808"));
    TEST_TRUE(hash(md5, std::string(622, 'a')+'\xa', "ad09e2739607c40e36fb75450263276f"));

    md5.clear();
    md5 << "aaa";
    auto h1 = md5.hash();
    md5.clear();
    md5 << "a" << "a" << "a";
    auto h2 = md5.hash();
    TEST_TRUE(h1 == h2);
        
    return 0;
}
