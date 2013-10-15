#include "gubg/Testing.hpp"
#include "gubg/parse/cpp/Parser.hpp"
using namespace gubg::parse::cpp;
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(Parser);
    Parser parser;
    TEST_OK(parser.parse(File(__FILE__)));
    int i    = 1;
    string str_123 = "abc\"def";
    char ch = '\'';
    //Some line comment "asdfasdf" 124 ... *///
    //
    /* 123"asdf"///*/
    return 0;
}
#include "gubg/log/end.hpp"
