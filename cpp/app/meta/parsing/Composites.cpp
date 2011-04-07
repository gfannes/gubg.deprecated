#include "parsing/Composites.hpp"
#include "OnlyOnce.hpp"
using namespace meta;

#define L_ENABLE_DEBUG
#include "debug.hpp"

Comment *Comment::tryCreate(TokenRange &tr)
{
    if (tr.size() >= 2 && tr[0]->isSymbol('/') && tr[1]->isSymbol('/'))
    {
        DEBUG_PRINT("Found a comment:");
        auto comment = new Comment;
        comment->add(tr[0]);
        comment->add(tr[1]);
        tr.advance_begin(2);
        while (!tr.empty())
        {
            auto token = tr.front();
            if (token->isEnd())
                break;
            comment->add(token);
            tr.pop_front();
            if (token->isNewline())
                break;
        }
        DEBUG_PRINT(toCode(CodeRange(comment->childs_.front()->range_.begin(), comment->childs_.back()->range_.end())));
        return comment;
    }
    return nullptr;
}

Include *Include::tryCreate(TokenRange &tr)
{
    char lch;
    if (tr.size() >= 6 && tr[0]->isSymbol('#') && tr[1]->isName("include") && tr[2]->isWhitespace() && tr[3]->isSymbol(lch, "\"<"))
    {
        DEBUG_PRINT("Found an include:");
        auto incl = new Include;
        tr.advance_begin(4);
        while (!tr.empty())
        {
            auto token = tr.front();
            tr.pop_front();
            char rch;
            if (token->isSymbol(rch, "\">") && (('\"' == lch && '\"' == rch) || ('<' == lch && '>' == rch)))
            {
                //We found the closing '"' or '>' symbol
                DEBUG_PRINT(toCode(CodeRange(incl->childs_.front()->range_.begin(), incl->childs_.back()->range_.end())));
                return incl;
            }
            incl->add(token);
        }
        gubg::Exception::raise(gubg::Exception("Could not find the closing tag for an include statement"));
    }
    return nullptr;
}

String *String::tryCreate(TokenRange &tr)
{
    if (tr.size() >= 2 && tr[0]->isSymbol('\"'))
    {
        DEBUG_PRINT("Found a string:");
        auto str = new String;
        tr.pop_front();
        //escapedCharater will be reset if we encounter a backslash, indicating that the next character is to be taken literally
        gubg::OnlyOnce escapedCharacter(false);
        while (!tr.empty())
        {
            auto token = tr.front();
            tr.pop_front();
            char ch;
            if (!escapedCharacter() && token->isSymbol(ch, "\\\""))
            {
                switch (ch)
                {
                    case '\"':
                        //This prints incorrectly, skipped escape-backslashes will be printed too...
                        DEBUG_PRINT(toCode(CodeRange(str->childs_.front()->range_.begin(), str->childs_.back()->range_.end())));
                        return str;
                        break;
                    case '\\':
                        escapedCharacter.reset();
                        continue;
                        break;
                }
            }
            str->add(token);
        }
        gubg::Exception::raise(gubg::Exception("Could not find the closing quote for an inline string"));
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
