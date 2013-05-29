#include "gubg/parse/xml/Coding.hpp"
#include "gubg/Testing.hpp"
using namespace gubg::xml;
using namespace std;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
namespace 
{
    bool checkEncodingOK(const string &raw, const string &wanted)
    {
        MSS_BEGIN(bool);
        string enc;
        MSS(encode(enc, raw));
        MSS(wanted == enc);
        MSS_END();
    }
    bool checkEncodingKO(const string &raw)
    {
        string enc;
        return !MSS_IS_OK(encode(enc, raw));
    }
    bool checkDecodingOK(const string &enc, const string &wanted)
    {
        MSS_BEGIN(bool);
        string raw;
        MSS(decode(raw, enc));
        MSS(wanted == raw);
        MSS_END();
    }
    bool checkDecodingKO(const string &enc)
    {
        string raw;
        return !MSS_IS_OK(decode(raw, enc));
    }
}
int main()
{
    TEST_TAG(main);
    {
        TEST_TAG(encode);
        {
            TEST_TAG(pos);
            TEST_TRUE(checkEncodingOK("", ""));
            TEST_TRUE(checkEncodingOK(" ", " "));
            TEST_TRUE(checkEncodingOK("\n", "\n"));
            TEST_TRUE(checkEncodingOK("\r", "\r"));
            TEST_TRUE(checkEncodingOK("\t", "\t"));
            TEST_TRUE(checkEncodingOK("abc", "abc"));
            TEST_TRUE(checkEncodingOK("\"", "&quot;"));
            TEST_TRUE(checkEncodingOK("<", "&lt;"));
            TEST_TRUE(checkEncodingOK(">", "&gt;"));
            TEST_TRUE(checkEncodingOK("&", "&amp;"));
        }
        {
            TEST_TAG(neg);
            TEST_TRUE(checkEncodingKO("\x01"));
        }
    }
    {
        TEST_TAG(decode);
        {
            TEST_TAG(pos);
            TEST_TRUE(checkDecodingOK("", ""));
            TEST_TRUE(checkDecodingOK(" ", " "));
            TEST_TRUE(checkDecodingOK("\n", "\n"));
            TEST_TRUE(checkDecodingOK("\r", "\r"));
            TEST_TRUE(checkDecodingOK("\t", "\t"));
            TEST_TRUE(checkDecodingOK("abc", "abc"));
            TEST_TRUE(checkDecodingOK("&quot;", "\""));
            TEST_TRUE(checkDecodingOK("&lt;", "<"));
            TEST_TRUE(checkDecodingOK("&gt;", ">"));
            TEST_TRUE(checkDecodingOK("&amp;", "&"));
            TEST_TRUE(checkDecodingOK("&amp;&amp;", "&&"));
        }
        {
            TEST_TAG(neg);
            TEST_TRUE(checkDecodingKO("<"));
            TEST_TRUE(checkDecodingKO(">"));
            TEST_TRUE(checkDecodingKO("\""));
            TEST_TRUE(checkDecodingKO("&"));
            TEST_TRUE(checkDecodingKO("&quot"));
            TEST_TRUE(checkDecodingKO("&abc"));
        }
    }
    return 0;
}
#include "gubg/log/end.hpp"
