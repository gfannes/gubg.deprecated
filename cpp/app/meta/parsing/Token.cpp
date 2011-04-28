#include "parsing/Token.hpp"
#include "boost/regex.hpp"
#include <cassert>
using namespace meta;
using namespace std;
using namespace boost;

//#define L_ENABLE_DEBUG
#include "debug.hpp"

namespace
{
    //Regular expressions
    regex reName("\\A[a-zA-Z_][a-zA-Z_\\d]*");
    regex reNumber("\\A\\d+");
    regex reWhitespace("\\A[ \\t]+");
}

Token::Ptr Token::construct(CodeRange &range)
{
    Token::Ptr res;
    if (range.empty())
        res.reset(new End(range));
    else if ('\n' == range.front())
        res.reset(new Newline(reduce(range, 1)));
    else
    {
        //We use an smatch here iso a cmatch, sometime, this function is called with a range
        //that is _not_ the full string, but that needs to end earlier (e.g., when we split a Name that comes after an escape-character)
        smatch match;
        auto str = toCode(range);
        if (regex_search(str, match, reName))
        {
            auto m = match[0];
            DEBUG_PRINT("Name: size: " << match.size() << ", match[0]: " << m << ", match[0].length(): " << m.length());
            res.reset(new Name(reduce(range, m.length())));
        }
        else if (regex_search(str, match, reNumber))
        {
            auto m = match[0];
            DEBUG_PRINT("Number: size: " << match.size() << ", match[0]: " << m << ", match[0].length(): " << m.length());
            res.reset(new Number(reduce(range, m.length())));
        }
        else if (regex_search(str, match, reWhitespace))
        {
            auto m = match[0];
            DEBUG_PRINT("Whitespace: size: " << match.size() << ", match[0]: " << m << ", match[0].length(): " << m.length());
            res.reset(new Whitespace(reduce(range, m.length())));
        }
        else
        {
            DEBUG_PRINT("Symbol: \"" << str[0] << "\"");
            res.reset(new Symbol(reduce(range, 1)));
        }
    }
    return res;
}

bool meta::popToken(Token::Ptr &token, TokenRange &tr)
{
    if (tr.empty())
        return false;
    token = tr.range.front();
    if (token->isEnd())
        return false;
    tr.pop_front();
    return true;
}
bool meta::popToken(Token::Ptr &token, ComponentRange &cr)
{
    if (cr.empty())
        return false;
    //Check if we have a Token at the front
    token = dynamic_pointer_cast<Token, Component>(cr.front());
    if (!token)
        return false;

    if (token->isEnd())
        return false;
    cr.pop_front();
    return true;
}

#ifdef UnitTest
int main()
{
    Code code("name1 \t 123.456 name2");
    CodeRange range(code);
    while (auto token = Token::construct(range))
    {
        if (token->isEnd())
            break;
    }

    return 0;
}
#endif
