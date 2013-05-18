#include "gubg/parse/xml/Parser.hpp"
#include "gubg/Testing.hpp"
#include "gubg/file/Filesystem.hpp"

namespace data
{
    const char * open_close = "<abc></abc>";
    const char * openclose = "<abc/>";
    const char * open_close_attr = "<abc id=\"1\"></abc>";
    const char * openclose_attr = "<abc id=\"1\"/>";
    const char * text = "<abc>text</abc>";
    const char * comment = "<abc><!--this is a comment--></abc>";
}

#define GUBG_MODULE_ "ConcreteParser"
#include "gubg/log/begin.hpp"
namespace 
{
    struct Parser: gubg::xml::Parser_crtp<Parser>
    {
        void parser_text(const std::string &text, const Path &path)
        {
            SS(text);
        }
        void parser_open(const std::string &tag, const Path &path)
        {
            SS(tag);
        }
        void parser_close(const std::string &tag, const Path &path)
        {
            SS(tag);
        }
        void parser_attr(const Attributes &attrs, const Path &path)
        {
            SS(attrs.size());
            for (auto attr: attrs)
            {
                L(STREAM(attr.first, attr.second));
            }
        }
        void parser_comment(const std::string &comment, const Path &path)
        {
            SS(comment);
        }
    };
}
#include "gubg/log/end.hpp"

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    Parser p;
    TEST_OK(p.process(data::open_close));
    TEST_OK(p.process(data::openclose));
    TEST_OK(p.process(data::open_close_attr));
    TEST_OK(p.process(data::openclose_attr));
    TEST_OK(p.process(data::text));
    TEST_OK(p.process(data::comment));
    if (true)
    {
        std::string str;
        gubg::file::read(str, gubg::file::File("/home/gfannes/cryptx-avalon/Overview.mm"));
        TEST_OK(p.process(str));
    }
    return 0;
}
#include "gubg/log/end.hpp"
