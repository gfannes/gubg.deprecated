#include "parsing/Parser.hpp"
using namespace meta;
using namespace std;

Structure Parser::parse(const std::string &code)
{
    Structure res(code);

    //Parse res.code_ into tokens
    {
        const char *chBegin = res.code_.c_str();
        while (*chBegin)
        {
            const char *chEnd = chBegin;
            if (parseNumber_(chEnd))
        }
    }


    return std::move(res);
}

#ifdef UnitTest
#include <string>
int main()
{
    string code("#include <test.h>");
    Parser parser;
    auto s = parser.parse(code);
    return 0;
}
#endif
