#include "parsing/Parser.hpp"
#include "parsing/Composites.hpp"
using namespace meta;
using namespace std;

Structure Parser::parse(Code &code)
{
    Structure res(code);

    //Parse res.code_ into tokens
    {
        CodeRange codeRange(res.code_);
        while (auto token = Token::tryCreate(codeRange))
        {
            res.tokens_.push_back(token);
            if (token->isEnd())
                break;
        }
    }

    //Initialize the TokenRange that will be used for creating the Composites
    TokenRange tokenRange(res.tokens_);
    if (tokenRange.empty())
        gubg::Exception::raise(EmptyCode());

    //Create the Composites
    while (!tokenRange.empty())
    {
        Component *component = 0;
        if (auto comment = Comment::tryCreate(tokenRange))
            component = comment;
        else
        {
            tokenRange.pop_front();
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
    Code code("//comment 123\n#include \"test.h\"");
    Parser parser;
    auto s = parser.parse(code);
    return 0;
}
#endif
