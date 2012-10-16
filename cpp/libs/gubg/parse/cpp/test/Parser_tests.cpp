#include "gubg/testing/Testing.hpp"
#include "gubg/parse/cpp/Parser.hpp"
using namespace gubg::parse::cpp;
using namespace gubg::file;

int main()
{
    TEST_TAG(Parser);
    Parser parser;
    TEST_OK(parser.parse(File(__FILE__)));
    return 0;
}
