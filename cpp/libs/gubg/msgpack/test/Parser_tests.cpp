#include "gubg/Testing.hpp"
#include "gubg/msgpack/Parser.hpp"
#include "gubg/msgpack/Write.hpp"
#include <string>
using namespace std;
using namespace gubg;
using namespace gubg::testing;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
namespace 
{
    using namespace gubg::msgpack;
    typedef vector<Element> Path;

    struct Parser: msgpack::Parser_crtp<Parser, Path>
    {
        void parser_open(Element el, const Path &path)
        {
            SS(el.ix, el.length);
        }
        void parser_close(Element el, const Path &path)
        {
            SS(el.ix, el.length);
        }
        void parser_add(long v, const Path &path)
        {
            SS(v);
            if (!path.empty())
                L(STREAM(path.back().ix));
        }
        void parser_add(unsigned long v, const Path &path)
        {
            SS(v, path.size());
            if (!path.empty())
                L(STREAM(path.back().ix));
        }
        void parser_add(char ch, const Path &path)
        {
            SS(ch);
            if (!path.empty())
                L(STREAM(path.back().ix));
        }
        template <typename T>
            void parser_add(const T &t, const Path &path)
            {
                SS(path.size());
            }
    };
}

int main()
{
    TEST_TAG(main);
    Parser parser;
    //0
    parser.process((unsigned char)0x00);
    //1
    parser.process((unsigned char)0x01);
    //uint16 0x1234
    parser.process((unsigned char)0xcd);
    parser.process((unsigned char)0x12);
    parser.process((unsigned char)0x34);
    //uint32 0x12345678
    parser.process((unsigned char)0xce);
    parser.process((unsigned char)0x12);
    parser.process((unsigned char)0x34);
    parser.process((unsigned char)0x56);
    parser.process((unsigned char)0x78);

    cout << endl;
    //[0, 1]
    parser.process((unsigned char)0x92);
    parser.process((unsigned char)0x00);
    parser.process((unsigned char)0x01);

    cout << endl;
    //"123"
    parser.process((unsigned char)0xa3);
    parser.process((unsigned char)0x31);
    parser.process((unsigned char)0x32);
    parser.process((unsigned char)0x33);

    if (false)
    for (int i = -1; i > -130; --i)
    {
        string str;
        msgpack::write(str, i);
        L(testing::toHex(str));
        for (char ch: str)
            parser.process((unsigned char)ch);
    }
    return 0;
}
#include "gubg/log/end.hpp"
