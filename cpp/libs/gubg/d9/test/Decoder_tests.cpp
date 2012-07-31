#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/testing/Testing.hpp"
#include "gubg/d9/Decoder.hpp"
#include <string>
#include <sstream>
using namespace gubg::d9;
using namespace gubg::testing;
using namespace std;

namespace 
{
    string str_(const char *str)
    {
        return string(str);
    }
    string str_(const vector<int> &bytes)
    {
        string str;
        for (auto byte: bytes)
            str.push_back((char)byte);
        return str;
    }
    ReturnCode checkDec_(const vector<int> &encoded, const string &wanted)
    {
        MSS_BEGIN(ReturnCode, checkDec_);
        string dec;
        Decoder<string, string> decoder(dec);
        for (auto v: encoded)
            decoder.process((ubyte)v);
        LOG_M("dec: " << toHex(dec));
        MSS(dec == wanted);
        MSS_END();
    }
}

int main()
{
    TEST_TAG(main);
    {
        TEST_TAG(Decoder);
        TEST_OK(checkDec_({0xd9,       0x80, 0x61, 0x62, 0x63,                         0xd9, 0xff}, str_("abc")));
        TEST_OK(checkDec_({0xd9,       0x80, 0xd8,                                     0xd9, 0xff}, str_("\xd8")));
        TEST_OK(checkDec_({0xd9,       0x81, 0xd8,                                     0xd9, 0xff}, str_("\xd9")));
        TEST_OK(checkDec_({0xd9,       0x83, 0xd8, 0xd8,                               0xd9, 0xff}, str_("\xd9\xd9")));
        TEST_OK(checkDec_({0xd9,       0x87, 0xd8, 0xd8, 0xd8,                         0xd9, 0xff}, str_("\xd9\xd9\xd9")));
        TEST_OK(checkDec_({0xd9,       0x8f, 0xd8, 0xd8, 0xd8, 0xd8,                   0xd9, 0xff}, str_("\xd9\xd9\xd9\xd9")));
        TEST_OK(checkDec_({0xd9,       0x9f, 0xd8, 0xd8, 0xd8, 0xd8, 0xd8,             0xd9, 0xff}, str_("\xd9\xd9\xd9\xd9\xd9")));
        TEST_OK(checkDec_({0xd9,       0xbf, 0xd8, 0xd8, 0xd8, 0xd8, 0xd8, 0xd8,       0xd9, 0xff}, str_("\xd9\xd9\xd9\xd9\xd9\xd9")));
        TEST_OK(checkDec_({0xd9, 0x7f, 0x80, 0xd8, 0xd8, 0xd8, 0xd8, 0xd8, 0xd8, 0xd8, 0xd9, 0xff}, str_("\xd9\xd9\xd9\xd9\xd9\xd9\xd9")));
    }
    return 0;
}

