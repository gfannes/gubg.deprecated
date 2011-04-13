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
        else if (auto incl = Include::tryCreate(tokenRange))
            component = incl;
        else if (auto str = String::tryCreate(tokenRange))
            component = str;
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
#include "Testing.hpp"
#include <string>
int main()
{
    Parser parser;

    Structure s;

    TEST_TAG(meta|parsing|Parser);
    TEST_REPORT_TYPE(Full);
    {
        {
            TEST_TAG(Comment);
            s = parser.parse(Code("//Comment 123"));
            TEST_EQ(1, s.components_.size());
            TEST_NOT_NULL(dynamic_cast<Comment*>(s.components_.front()));
            s = parser.parse(Code("//Comment 123\n"));
            TEST_EQ(1, s.components_.size());
            TEST_NOT_NULL(dynamic_cast<Comment*>(s.components_.front()));
        }

        {
            TEST_TAG(Include);
            s = parser.parse(Code("#include \"test.h\""));
            TEST_EQ(1, s.components_.size());
            TEST_NOT_NULL(dynamic_cast<Include*>(s.components_.front()));
            s = parser.parse(Code("#include <test.h>"));
            TEST_EQ(1, s.components_.size());
            TEST_NOT_NULL(dynamic_cast<Include*>(s.components_.front()));
        }

        {
            TEST_TAG(String);
            s = parser.parse(Code("\"inline string\""));
            TEST_EQ(1, s.components_.size());
            TEST_NOT_NULL(dynamic_cast<String*>(s.components_.front()));
            s = parser.parse(Code("\"\\\"\\nbla\\\\\""));
            TEST_EQ(1, s.components_.size());
            TEST_NOT_NULL(dynamic_cast<String*>(s.components_.front()));
        }
    }
    return 0;
}
#endif
