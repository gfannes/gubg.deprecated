#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/testing/Testing.hpp"
#include "gubg/msgpack/Parser.hpp"
#include <string>
using namespace std;
using namespace gubg;
using namespace gubg::testing;

namespace 
{
    using namespace gubg::msgpack;
    typedef vector<Element> Path;

    struct Parser: msgpack::Parser_crtp<Parser, Path>
    {
        ReturnCode parser_open(Element el, const Path &path)
        {
            LOG_S(open, STREAM(el.ix, el.length));
            return ReturnCode::OK;
        }
        ReturnCode parser_close(Element el, const Path &path)
        {
            LOG_S(close, STREAM(el.ix, el.length));
            return ReturnCode::OK;
        }
        ReturnCode parser_add(long v, const Path &path)
        {
            LOG_S(add_l, STREAM(v));
            if (!path.empty())
                LOG_M(STREAM(path.back().ix));
            return ReturnCode::OK;
        }
        ReturnCode parser_add(unsigned long v, const Path &path)
        {
            LOG_S(add_ul, STREAM(v));
            if (!path.empty())
                LOG_M(STREAM(path.back().ix));
            return ReturnCode::OK;
        }
        template <typename T>
        ReturnCode parser_add(const T &t, const Path &path)
        {
            return ReturnCode::IllegalArgument;
        }
    };
}

int main()
{
    TEST_TAG(main);
    Parser parser;
    //0
    parser.process(0x00);
    //1
    parser.process(0x01);
    //uint16 0x1234
    parser.process(0xcd);
    parser.process(0x12);
    parser.process(0x34);
     //uint32 0x12345678
    parser.process(0xce);
    parser.process(0x12);
    parser.process(0x34);
    parser.process(0x56);
    parser.process(0x78);

    cout << endl;
    //[0, 1]
    parser.process(0x92);
    parser.process(0x00);
    parser.process(0x01);
    return 0;
}
