#include "parsing/Composites.hpp"
using namespace meta;

#define L_ENABLE_DEBUG
#include "debug.hpp"

Comment *Comment::tryCreate(TokenRange &tokens)
{
    if (tokens.size() >= 2 && tokens[0]->isSymbol('/') && tokens[1]->isSymbol('/'))
    {
        DEBUG_PRINT("Found a comment:");
        auto comment = new Comment;
        comment->add(tokens[0]);
        comment->add(tokens[1]);
        tokens.advance_begin(2);
        while (!tokens.empty())
        {
            auto token = tokens.front();
            if (token->isEnd())
                break;
            comment->add(token);
            tokens.pop_front();
            if (token->isNewline())
                break;
        }
        DEBUG_PRINT(toCode(CodeRange(comment->childs_.front()->range_.begin(), comment->childs_.back()->range_.end())));
        return comment;
    }
    return nullptr;
}

#ifdef UnitTest
int main()
{
    Code code("//bla bli blo\nflap");
    CodeRange cr(code);
    Tokens tokens;
    tokens.push_back(new Symbol(reduce(cr, 1)));
    tokens.push_back(new Symbol(reduce(cr, 1)));
    tokens.push_back(new Name(reduce(cr, 3)));
    tokens.push_back(new Whitespace(reduce(cr, 1)));
    tokens.push_back(new Name(reduce(cr, 3)));
    tokens.push_back(new Whitespace(reduce(cr, 1)));
    tokens.push_back(new Name(reduce(cr, 3)));
    tokens.push_back(new Newline(reduce(cr, 1)));
    tokens.push_back(new Name(reduce(cr, 4)));
    TokenRange tr(tokens);
    auto comment = Comment::tryCreate(tr);
    return 0;
}
#endif
