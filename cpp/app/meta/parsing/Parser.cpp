#include "parsing/Parser.hpp"
#include "parsing/Composites.hpp"
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

    //Initialize the TokenRange that will be used for creating the Composites
    TokenRange range(res.tokens_);
    if (range.empty())
        gubg::Exception::raise(EmptyCode());

    //Create the Composites
    while (!range.empty())
    {
        Component *component = 0;
        if (auto comment = Comment::tryCreate(range))
            component = comment;
        else
        {
            range.advance_begin(1);
            continue;
        }
        res.components_.push_back(component);
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
