#include "parsing/Parser.hpp"
using namespace meta;
using namespace std;

Structure Parser::parse(const std::string &code)
{
    Structure res(code);

    //Parse res.code_ into tokens
    {
        const char *ch = res.code_.c_str();
        while (auto token = Token::tryCreate(ch))
        {
            res.tokens_.push_back(token);
            if (token->isEnd())
                break;
        }
    }

    return std::move(res);
}

#ifdef UnitTest
#include <string>
int main()
{
    string code("//comment 123\n#include <test.h>");
    Parser parser;
    auto s = parser.parse(code);
    return 0;
}
#endif
