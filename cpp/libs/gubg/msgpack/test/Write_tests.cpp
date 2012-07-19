#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/testing/Testing.hpp"
#include "gubg/msgpack/Write.hpp"
#include <string>
using namespace std;
using namespace gubg;
using namespace gubg::testing;

namespace 
{
    string pack(int v)
    {
        string str;
        msgpack::write(str, v);
        LOG_S(pack, toHex(str));
        return str;
    }
    string str_(const vector<int> &bytes)
    {
        string str;
        for (auto byte: bytes)
            str.push_back((char)byte);
        return str;
    }
}

int main()
{
    TEST_TAG(main);
    TEST_EQ(str_({0x7f}), pack(127));
    TEST_EQ(str_({0xcc, 0x80}), pack(128));
    TEST_EQ(str_({0xcd, 0x01, 0x00}), pack(256));
    TEST_EQ(str_({0xcd, 0x01, 0x01}), pack(257));
    TEST_EQ(str_({0xce, 0x00, 0x01, 0x00, 0x00}), pack(65536));
    TEST_EQ(str_({0xce, 0x7f, 0xff, 0xff, 0xff}), pack(0x7fffffff));
    return 0;
}
