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
    regex reName("^[a-zA-Z_][a-zA-Z_\\d]*");
    regex reNumber("^\\d+");
    regex reWhitespace("^[ \\t]+");
}

Token *Token::tryCreate(CodeRange &range)
{
    if (range.empty()) 
        return new End(range);
    if ('\n' == range.front())
        return new Newline(reduce(range, 1));

    //We use an smatch here iso a cmatch, sometime, this function is called with a range
    //that is _not_ the full string, but that needs to end earlier (e.g., when we split a Name that comes after an escape-character)
    smatch match;
    auto str = toCode(range);
    if (regex_search(str, match, reName))
    {
        auto m = match[0];
        DEBUG_PRINT("Name: size: " << match.size() << ", match[0]: " << m << ", match[0].length(): " << m.length());
        return new Name(reduce(range, m.length()));
    }
    if (regex_search(str, match, reNumber))
    {
        auto m = match[0];
        DEBUG_PRINT("Number: size: " << match.size() << ", match[0]: " << m << ", match[0].length(): " << m.length());
        return new Number(reduce(range, m.length()));
    }
    if (regex_search(str, match, reWhitespace))
    {
        auto m = match[0];
        DEBUG_PRINT("Whitespace: size: " << match.size() << ", match[0]: " << m << ", match[0].length(): " << m.length());
        return new Whitespace(reduce(range, m.length()));
    }

    DEBUG_PRINT("Symbol: \"" << str[0] << "\"");
    return new Symbol(reduce(range, 1));
}

#ifdef UnitTest
int main()
{
    Code code("name1 \t 123.456 name2");
    CodeRange range(code);
    while (auto token = Token::tryCreate(range))
    {
        if (token->isEnd())
            break;
    }

    return 0;
}
#endif
