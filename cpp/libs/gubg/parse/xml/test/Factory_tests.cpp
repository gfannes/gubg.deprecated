#include "gubg/parse/xml/Factory.hpp"
#include "gubg/Testing.hpp"
#include "gubg/file/Filesystem.hpp"

namespace data
{
    const char * open_close = "<abc></abc>";
    const char * openclose = "<abc/>";
    const char * open_close_attr = "<abc id=1></abc>";
    const char * openclose_attr = "<abc id=1/>";
    const char * text = "<abc>text</abc>";
    const char * comment = "<abc><!--this is a comment--></abc>";
}

#define GUBG_MODULE_ "ConcreteFactory"
#include "gubg/log/begin.hpp"
namespace 
{
    struct Factory: gubg::xml::Factory_crtp<Factory>
    {
        void factory_text(const std::string &text, const Path &path)
        {
            SS(text);
        }
        void factory_open(const std::string &tag, const Path &path)
        {
            SS(tag);
        }
        void factory_close(const std::string &tag, const Path &path)
        {
            SS(tag);
        }
        void factory_comment(const std::string &comment, const Path &path)
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
    Factory f;
    TEST_OK(f.process(data::open_close));
    TEST_OK(f.process(data::openclose));
    TEST_OK(f.process(data::open_close_attr));
    TEST_OK(f.process(data::openclose_attr));
    TEST_OK(f.process(data::text));
    TEST_OK(f.process(data::comment));
    if (false)
    {
        std::string str;
        gubg::file::read(str, gubg::file::File("/home/gfannes/cryptx-avalon/Overview.mm"));
        TEST_OK(f.process(str));
    }
    return 0;
}
#include "gubg/log/end.hpp"
