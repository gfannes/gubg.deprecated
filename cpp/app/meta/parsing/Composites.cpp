#include "parsing/Composites.hpp"
using namespace meta;

#define L_ENABLE_DEBUG
#include "debug.hpp"

Comment *Comment::tryCreate(TokenRange &tokens)
{
    if (tokens.size() >= 2 && tokens[0]->isSymbol('/') && tokens[1]->isSymbol('/'))
    {
        DEBUG_PRINT("Found a comment");
        auto comment = new Comment;
        comment->add(tokens[0]);
        comment->add(tokens[1]);
        tokens.advance_begin(2);
        return comment;
    }
    return nullptr;
}

#ifdef UnitTest
int main()
{
    const char *str = "//";
    Structure::Tokens tokens;
    tokens.push_back(new Symbol(str));
    tokens.push_back(new Symbol(str));
    TokenRange range(tokens);
    auto comment = Comment::tryCreate(range);
    return 0;
}
#endif
