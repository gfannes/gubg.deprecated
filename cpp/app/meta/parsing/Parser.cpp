#include "parsing/Parser.hpp"
#include "parsing/Composite.hpp"
using namespace meta;
using namespace std;

#define L_ENABLE_DEBUG
#include "debug.hpp"

Structure::Ptr Parser::parse(Code &code)
{
    Structure::Ptr res(new Structure(code));
    auto &stru = *res;

    //Parse stru.code_ into tokens
    {
        CodeRange codeRange(stru.code_);
        while (auto token = Token::construct(codeRange))
        {
            stru.tokens_.push_back(token);
            if (token->isEnd())
                break;
        }
    }

    //Initialize the TokenRange that will be used for creating the Composites
    TokenRange tokenRange(stru.tokens_);
    if (tokenRange.empty())
        gubg::Exception::raise(EmptyCode());

    //Create the Composites
    while (!tokenRange.empty())
    {
        Component::Ptr component;
        if (auto comment = Comment::construct(tokenRange))
            component = comment;
        else if (auto incl = Include::construct(tokenRange))
            component = incl;
        else if (auto def = Define::construct(tokenRange))
            component = def;
        else if (auto str = String::construct(tokenRange))
            component = str;
        else if (auto ch = Character::construct(tokenRange))
            component = ch;
        else
        {
            auto token = tokenRange.front();
            tokenRange.pop_front();
            if (token->isEnd()){}//We filter the end
            else if (token->isWhitespace()){}//We filter whitespaces
            else
                component = token;
        }
        if (component)
            stru.components_.push_back(component);
    }

    return res;
}

#ifdef UnitTest
#include "Testing.hpp"
#include <string>
int main()
{
    Parser parser;

    Structure::Ptr s;

    TEST_TAG(meta|parsing|Parser);
    TEST_REPORT_TYPE(Full);
    {
        {
            TEST_TAG(Comment);
            s = parser.parse(Code("//Comment 123"));
            TEST_TRUE(s);
            TEST_EQ(1, s->components_.size());
            TEST_NOT_NULL(dynamic_cast<Comment*>(s->components_.front().get()));
            s = parser.parse(Code("//Comment 123\n"));
            TEST_TRUE(s);
            TEST_EQ(1, s->components_.size());
            TEST_NOT_NULL(dynamic_cast<Comment*>(s->components_.front().get()));
        }

        {
            TEST_TAG(Include);
            s = parser.parse(Code("#include \"test.h\""));
            TEST_TRUE(s);
            TEST_EQ(1, s->components_.size());
            TEST_NOT_NULL(dynamic_cast<Include*>(s->components_.front().get()));
            s = parser.parse(Code("#include <test.h>"));
            TEST_TRUE(s);
            TEST_EQ(1, s->components_.size());
            TEST_NOT_NULL(dynamic_cast<Include*>(s->components_.front().get()));
        }

        {
            TEST_TAG(Define);
            s = parser.parse(Code("#define ABC"));
            TEST_TRUE(s);
            TEST_EQ(1, s->components_.size());
            TEST_NOT_NULL(dynamic_cast<Define*>(s->components_.front().get()));
            s = parser.parse(Code("#define ABC \\\n\tbla\\\nbli\nfrot"));
            TEST_TRUE(s);
            TEST_EQ(2, s->components_.size());
            TEST_NOT_NULL(dynamic_cast<Define*>(s->components_.front().get()));
        }

        {
            TEST_TAG(String);
            s = parser.parse(Code("\"inline string\""));
            TEST_TRUE(s);
            TEST_EQ(1, s->components_.size());
            TEST_NOT_NULL(dynamic_cast<String*>(s->components_.front().get()));
            s = parser.parse(Code("\"\\\"\\nbla\\\\\""));
            TEST_TRUE(s);
            TEST_EQ(1, s->components_.size());
            TEST_NOT_NULL(dynamic_cast<String*>(s->components_.front().get()));
        }

        {
            TEST_TAG(Character);
            s = parser.parse(Code("\'a\'"));
            TEST_TRUE(s);
            TEST_EQ(1, s->components_.size());
            TEST_NOT_NULL(dynamic_cast<Character*>(s->components_.front().get()));
            s = parser.parse(Code("\'\\\"\'"));
            TEST_TRUE(s);
            TEST_EQ(1, s->components_.size());
            TEST_NOT_NULL(dynamic_cast<Character*>(s->components_.front().get()));
        }
    }
    return 0;
}
#endif
