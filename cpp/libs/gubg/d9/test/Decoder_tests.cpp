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
        StringDecoder<string> decoder;
        for (auto v: encoded)
            decoder.process((ubyte)v);
        LOG_M("dec: " << toHex(decoder.str()));
        MSS(decoder.str() == wanted);
        MSS_END();
    }
}

int main()
{
    TEST_TAG(main);
    {
        TEST_TAG(Decoder);
        TEST_OK(checkDec_({0xd9,          0xc0},                                         str_("\xc0")));
        TEST_OK(checkDec_({0xd9,     0x00,0xa1,0xd8},                                    str_("\xa1\xd8")));
        TEST_OK(checkDec_({0xd9,     0x01,0xa1,0xd8},                                    str_("\xa1\xd9")));
        TEST_OK(checkDec_({0xd9,     0x03,0xa2,0xd8,0xd8},                               str_("\xa2\xd9\xd9")));
        TEST_OK(checkDec_({0xd9,     0x07,0xa3,0xd8,0xd8,0xd8},                          str_("\xa3\xd9\xd9\xd9")));
        TEST_OK(checkDec_({0xd9,     0x0f,0xa4,0xd8,0xd8,0xd8,0xd8},                     str_("\xa4\xd9\xd9\xd9\xd9")));
        TEST_OK(checkDec_({0xd9,     0x1f,0xa5,0xd8,0xd8,0xd8,0xd8,0xd8},                str_("\xa5\xd9\xd9\xd9\xd9\xd9")));
        TEST_OK(checkDec_({0xd9,     0x3f,0xa6,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8},           str_("\xa6\xd9\xd9\xd9\xd9\xd9\xd9")));
        TEST_OK(checkDec_({0xd9,     0x7f,0xa7,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8},      str_("\xa7\xd9\xd9\xd9\xd9\xd9\xd9\xd9")));
        TEST_OK(checkDec_({0xd9,0x7f,0x01,0xa8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8}, str_("\xa8\xd9\xd9\xd9\xd9\xd9\xd9\xd9\xd9")));
    }
    return 0;
}

