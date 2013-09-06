#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/Testing.hpp"
#include "gubg/d9/D9.hpp"
#include <string>
#include <sstream>
using namespace gubg::d9;
using namespace gubg::testing;
using namespace std;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
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
        TEST_OK(encode(enc, str_("\xa3" "abc")));
        LOG_M(toHex(enc));

        //Encoding of a msgpack FixPos is not allowed as this clashes with the flip encoding
        TEST_KO(encode(enc, str_("\x00")));

        TEST_OK(checkEnc_({0xd9,          0xc0},                                         str_("\xc0")));
        TEST_OK(checkEnc_({0xd9,     0x00,0xa1,0xd8},                                    str_("\xa1\xd8")));
        TEST_OK(checkEnc_({0xd9,     0x01,0xa1,0xd8},                                    str_("\xa1\xd9")));
        TEST_OK(checkEnc_({0xd9,     0x03,0xa2,0xd8,0xd8},                               str_("\xa2\xd9\xd9")));
        TEST_OK(checkEnc_({0xd9,     0x07,0xa3,0xd8,0xd8,0xd8},                          str_("\xa3\xd9\xd9\xd9")));
        TEST_OK(checkEnc_({0xd9,     0x0f,0xa4,0xd8,0xd8,0xd8,0xd8},                     str_("\xa4\xd9\xd9\xd9\xd9")));
        TEST_OK(checkEnc_({0xd9,     0x1f,0xa5,0xd8,0xd8,0xd8,0xd8,0xd8},                str_("\xa5\xd9\xd9\xd9\xd9\xd9")));
        TEST_OK(checkEnc_({0xd9,     0x3f,0xa6,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8},           str_("\xa6\xd9\xd9\xd9\xd9\xd9\xd9")));
        TEST_OK(checkEnc_({0xd9,     0x7f,0xa7,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8},      str_("\xa7\xd9\xd9\xd9\xd9\xd9\xd9\xd9")));
        TEST_OK(checkEnc_({0xd9,0x7f,0x01,0xa8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8}, str_("\xa8\xd9\xd9\xd9\xd9\xd9\xd9\xd9\xd9")));
    }
    {
        TEST_TAG(decode);
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
#include "gubg/log/end.hpp"
