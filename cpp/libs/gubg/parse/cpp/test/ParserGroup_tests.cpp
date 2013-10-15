#include "gubg/Testing.hpp"
#include "gubg/parse/cpp/ParserGroup.hpp"

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
namespace 
{
    struct Parser: gubg::parse::cpp::ParserGroup_crtp<Parser, gubg::parse::cpp::Identifier_crtp>
    {
	    template <typename String>
            void identifier_detected(const String &str)
            {
                S();L(STREAM(str));
            }
    };
}

int main()
{
    TEST_TAG(main);
    Parser parser;
    parser(gubg::file::File(__FILE__));
    return 0;
}
#include "gubg/log/end.hpp"
