#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/testing/Testing.hpp"
#include "gubg/msgpack/Read.hpp"
#include <string>
#include <vector>
using namespace std;
using namespace gubg;

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
    }
    return 0;
}
