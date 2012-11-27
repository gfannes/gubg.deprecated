#include "gubg/testing/Testing.hpp"
#include "gubg/parse/cpp/ParserGroup.hpp"
#include "gubg/l.hpp"

namespace 
{
    struct Parser: gubg::parse::cpp::ParserGroup_crtp<Parser, gubg::parse::cpp::Identifier_crtp>
    {
	    template <typename String>
            void identifier_detected(const String &str)
            {
                L(STREAM(str));
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
