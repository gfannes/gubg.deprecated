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
    template <typename T>
        string pack(T v)
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
    {
        TEST_TAG(integers);
        {
            TEST_TAG(positives);
            TEST_EQ(str_({0x00}), pack(0));
            TEST_EQ(str_({0x01}), pack(1));
            TEST_EQ(str_({0x7f}), pack(127));
            TEST_EQ(str_({0xcc, 0x80}), pack(128));
            TEST_EQ(str_({0xcd, 0x01, 0x00}), pack(256));
            TEST_EQ(str_({0xcd, 0x01, 0x01}), pack(257));
            TEST_EQ(str_({0xce, 0x00, 0x01, 0x00, 0x00}), pack(65536));
            TEST_EQ(str_({0xce, 0x7f, 0xff, 0xff, 0xff}), pack(0x7fffffff));
        }
        {
            TEST_TAG(negatives);
            TEST_EQ(str_({0xff}), pack(-1));
            TEST_EQ(str_({0xe0}), pack(-32));
            TEST_EQ(str_({0xd0, 0x80}), pack(-128));
            TEST_EQ(str_({0xd1, 0xff, 0x7f}), pack(-129));
            TEST_EQ(str_({0xd1, 0x80, 0x00}), pack(-32768));
            TEST_EQ(str_({0xd2, 0xff, 0xff,0x7f, 0xff}), pack(-32769));
            TEST_EQ(str_({0xd2, 0x80, 0x00, 0x00, 0x00}), pack(-2147483648LL));
        }
    }
    {
        TEST_TAG(nil);
        TEST_EQ(str_({0xc0}), pack(msgpack::Nil_tag()));
    }
    {
        TEST_TAG(boolean);
        TEST_EQ(str_({0xc3}), pack(true));
        TEST_EQ(str_({0xc2}), pack(false));
    }
    {
        TEST_TAG(array);
        {
            vector<bool> vec = {true, false, true, true};
            TEST_EQ(str_({0x94, 0xc3, 0xc2, 0xc3, 0xc3}), pack(vec));
        }
        {
            vector<int> vec = {0, 1, 2, 3, 4, 0x7fffffff};
            TEST_EQ(str_({0x96, 0x00, 0x01, 0x02, 0x03, 0x04, 0xce, 0x7f, 0xff, 0xff, 0xff}), pack(vec));
        }
    }
    return 0;
}
