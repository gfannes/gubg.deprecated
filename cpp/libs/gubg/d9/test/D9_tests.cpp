#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/testing/Testing.hpp"
#include "gubg/d9/D9.hpp"
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
    ReturnCode checkEnc_(const vector<int> &wanted, const string &plain)
    {
        MSS_BEGIN(ReturnCode, checkEnc_);
        string encoded;
        MSS(encode(encoded, plain));
        LOG_M(toHex(plain) << " encodes into " << toHex(encoded));
        MSS(encoded == str_(wanted));
        MSS_END();
    }
    ReturnCode checkDec_(const vector<int> &encoded, const string &wanted)
    {
        MSS_BEGIN(ReturnCode, checkDec_);
        string dec;
        MSS(decode(dec, str_(encoded)));
        LOG_M(toHex(str_(encoded)) << " decodes into " << toHex(dec));
        MSS(wanted == dec);
        MSS_END();
    }
}

int main()
{
    TEST_TAG(main);
    {
        TEST_TAG(encode);
        string enc;
        TEST_OK(encode(enc, str_("abc")));
        LOG_M(toHex(enc));

        TEST_OK(checkEnc_({0xd9,       0x80, 0x61, 0x62, 0x63,                         0xd9, 0xff}, str_("abc")));
        TEST_OK(checkEnc_({0xd9,       0x80, 0xd8,                                     0xd9, 0xff}, str_("\xd8")));
        TEST_OK(checkEnc_({0xd9,       0x81, 0xd8,                                     0xd9, 0xff}, str_("\xd9")));
        TEST_OK(checkEnc_({0xd9,       0x83, 0xd8, 0xd8,                               0xd9, 0xff}, str_("\xd9\xd9")));
        TEST_OK(checkEnc_({0xd9,       0x87, 0xd8, 0xd8, 0xd8,                         0xd9, 0xff}, str_("\xd9\xd9\xd9")));
        TEST_OK(checkEnc_({0xd9,       0x8f, 0xd8, 0xd8, 0xd8, 0xd8,                   0xd9, 0xff}, str_("\xd9\xd9\xd9\xd9")));
        TEST_OK(checkEnc_({0xd9,       0x9f, 0xd8, 0xd8, 0xd8, 0xd8, 0xd8,             0xd9, 0xff}, str_("\xd9\xd9\xd9\xd9\xd9")));
        TEST_OK(checkEnc_({0xd9,       0xbf, 0xd8, 0xd8, 0xd8, 0xd8, 0xd8, 0xd8,       0xd9, 0xff}, str_("\xd9\xd9\xd9\xd9\xd9\xd9")));
        TEST_OK(checkEnc_({0xd9, 0x7f, 0x80, 0xd8, 0xd8, 0xd8, 0xd8, 0xd8, 0xd8, 0xd8, 0xd9, 0xff}, str_("\xd9\xd9\xd9\xd9\xd9\xd9\xd9")));
    }
    {
        TEST_TAG(decode);
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
