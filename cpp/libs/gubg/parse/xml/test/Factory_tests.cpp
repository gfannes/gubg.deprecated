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
}

namespace 
{
    struct Factory: gubg::xml::Factory_crtp<Factory>
    {
    };
}

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
    std::string str;
    gubg::file::read(str, gubg::file::File("/home/gfannes/cryptx-avalon/Overview.mm"));
    TEST_OK(f.process(str));
    return 0;
}
#include "gubg/log/end.hpp"
