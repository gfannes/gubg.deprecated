#include "gubg/Testing.hpp"
#include "gubg/msgpack/Read.hpp"
#include <string>
#include <vector>
using namespace std;
using namespace gubg;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
namespace 
{
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
    {
        TEST_TAG(integer);
        int i;
        msgpack::read(i, str_({0x00}));
        TEST_EQ(0, i);
        msgpack::read(i, str_({0x2a}));
        TEST_EQ(42, i);
        msgpack::read(i, str_({0xff}));
        TEST_EQ(-1, i);
        msgpack::read(i, str_({0xf0}));
        TEST_EQ(-16, i);
        msgpack::read(i, str_({0xef}));
        TEST_EQ(-17, i);
        msgpack::read(i, str_({0xed}));
        TEST_EQ(-19, i);
        msgpack::read(i, str_({0xe0}));
        TEST_EQ(-32, i);
        msgpack::read(i, str_({0xd0, 0xdf}));
        TEST_EQ(-33, i);
        msgpack::read(i, str_({0xd0, 0xde}));
        TEST_EQ(-34, i);
        msgpack::read(i, str_({0xd0, 0x80}));
        TEST_EQ(-128, i);
        msgpack::read(i, str_({0xd1, 0xff, 0x7f}));
        TEST_EQ(-129, i);
    }
    return 0;
}
#include "gubg/log/end.hpp"
