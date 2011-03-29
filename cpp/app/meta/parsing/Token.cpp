#include "parsing/Token.hpp"
#include <cassert>
#include "boost/regex.hpp"
using namespace meta;
using namespace std;
using namespace boost;

#define L_ENABLE_DEBUG
#include "debug.hpp"

namespace
{
    regex reName("^[a-zA-Z_][a-zA-Z_\\d]*");
    regex reNumber("^\\d+");
    regex reWhitespace("^[ \\t]+");
}

Token *Token::tryCreate(const char *&ch)
{
    assert(ch);

    if ('\0' == *ch) 
        return new End(ch);
    if ('\n' == *ch)
        return new Newline(ch++);

    cmatch match;
    if (regex_search(ch, match, reName))
    {
        auto m = match[0];
        DEBUG_PRINT("Name: size: " << match.size() << ", match[0]: " << m << ", match[0].length(): " << m.length());
        auto start = ch;
        ch += m.length();
        return new Name(start, ch);
    }
    if (regex_search(ch, match, reNumber))
    {
        auto m = match[0];
        DEBUG_PRINT("Number: size: " << match.size() << ", match[0]: " << m << ", match[0].length(): " << m.length());
        auto start = ch;
        ch += m.length();
        return new Number(start, ch);
    }
    if (regex_search(ch, match, reWhitespace))
    {
        auto m = match[0];
        DEBUG_PRINT("Whitespace: size: " << match.size() << ", match[0]: " << m << ", match[0].length(): " << m.length());
        auto start = ch;
        ch += m.length();
        return new Name(start, ch);
    }

    DEBUG_PRINT("Symbol: \"" << *ch << "\"");
    return new Symbol(ch++);
}

#ifdef UnitTest
int main()
{
    const char *ch = "name1 \t 123.456 name2";
    while (auto token = Token::tryCreate(ch))
    {
        if (token->isEnd())
            break;
    }

    return 0;
}
#endif
