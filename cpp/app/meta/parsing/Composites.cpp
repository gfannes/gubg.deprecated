#include "parsing/Composites.hpp"
#include "OnlyOnce.hpp"
using namespace meta;

//#define L_ENABLE_DEBUG
#include "debug.hpp"

namespace
{
    struct RestoreRange
    {
        RestoreRange(TokenRange &r):
            orig_(r),
            ref_(r),
            doRestore_(true){}
        ~RestoreRange()
        {
            if (doRestore_)
                ref_.range = orig_.range;
        }

        void commit(){doRestore_ = false;}

        TokenRange orig_;
        TokenRange &ref_;
        bool doRestore_;
    };

    //Returns true if a token (!= from End Token) could be popped
    bool popToken(Token *&token, TokenRange &tr)
    {
        if (tr.empty())
            return false;
        token = tr.range.front();
        if (token->isEnd())
            return false;
        tr.pop_front();
        return true;
    }
}

Comment *Comment::construct(TokenRange &tr)
{
    RestoreRange rr(tr);
    Token *token;
    if (popToken(token, tr) && token->isSymbol('/') && popToken(token, tr) && token->isSymbol('/'))
    {
        DEBUG_PRINT("Found a comment:");
        auto comment = new Comment;
        while (popToken(token, tr))
        {
            comment->add(token);
            if (token->isNewline())
                break;
        }
        DEBUG_PRINT(comment->toString());
        rr.commit();
        return comment;
    }
    return nullptr;
}

Include *Include::construct(TokenRange &tr)
{
    RestoreRange rr(tr);
    Token *token;
    char lch;
    if (popToken(token, tr) && token->isSymbol('#') &&
            popToken(token, tr) && token->isName("include") &&
            popToken(token, tr) && token->isWhitespace() && 
            popToken(token, tr) && token->isSymbol(lch, "\"<"))
    {
        DEBUG_PRINT("Found an include:");
        auto incl = new Include;
        while (popToken(token, tr))
        {
            char rch;
            if (token->isSymbol(rch, "\">") && (('\"' == lch && '\"' == rch) || ('<' == lch && '>' == rch)))
            {
                //We found the closing '"' or '>' symbol
                DEBUG_PRINT(incl->toString());
                rr.commit();
                return incl;
            }
            incl->add(token);
        }
        gubg::Exception::raise(gubg::Exception("Could not find the closing tag for an include statement"));
    }
    return nullptr;
}

Define *Define::construct(TokenRange &tr)
{
    RestoreRange rr(tr);
    Token *token;
    if (popToken(token, tr) && token->isSymbol('#') &&
            popToken(token, tr) && token->isName("define") &&
            popToken(token, tr) && token->isWhitespace())
    {
        DEBUG_PRINT("Found a define:");
        auto def = new Define;
        bool continueOnNextLine = false;
        while (popToken(token, tr))
        {
            if (token->isNewline())
            {
                if (!continueOnNextLine)
                    break;
                continueOnNextLine = false;
            }
            else if (token->isSymbol('\\'))
            {
                //We saw a '\', with our current knowledge, we have to continue on the next line
                continueOnNextLine = true;
            }
            else if (token->isWhitespace())
            {
                //A whitespace does not modify the fact that we have to continue on the next line
            }
            else
            {
                //All other tokens _do_ modify if we have to continue
                continueOnNextLine = false;
            }
            def->add(token);
        }
        DEBUG_PRINT(def->toString());
        rr.commit();
        return def;
    }
    return nullptr;
}

String *String::construct(TokenRange &tr)
{
    RestoreRange rr(tr);
    Token *token;
    if (popToken(token, tr) && token->isSymbol('\"'))
    {
        DEBUG_PRINT("Found a string:");
        auto str = new String;
        while (popToken(token, tr))
        {
            char ch;
            if (token->isSymbol(ch, "\\\""))
            {
                switch (ch)
                {
                    case '\"':
                        DEBUG_PRINT(str->toString());
                        rr.commit();
                        return str;
                        break;
                    case '\\':
                        str->add(token);
                        //The next character is escaped, but it might be part of a longer name or number, in which case we have to split it
                        //e.g. "bla\nbla" => a Name reading nbla is _not_ what we want
                        //We keep an iterator to the Token that we have to split
                        auto it = tr.range.begin();
                        if (!popToken(token, tr))
                                gubg::Exception::raise(gubg::Exception("No token following an escape character"));
                        if (token->range_.empty())
                            gubg::Exception::raise(gubg::Exception("Empty token following an escape character"));
                        //Split the first character of the Token
                        auto cr = reduce(token->range_, 1);
                        //Create and insert the new token. We do not attempt to translate it into the real, unescaped character
                        //This would be problematic, either changing the original Code, or storing this new character somewhere else
                        auto escape = Token::construct(cr);
                        tr.tokens.insert(it, escape);
                        //If the original Token without the first character is empty now, we remove it from the Tokens list
                        if (token->range_.empty())
                        {
                            tr.tokens.erase(it);
                            delete token;
                            //token will be added hereunder
                            token = escape;
                        }
                        else
                            str->add(escape);
                        break;
                }
            }
            str->add(token);
        }
        gubg::Exception::raise(gubg::Exception("Could not find the closing quote for an inline string"));
    }
    return nullptr;
}

Character *Character::construct(TokenRange &tr)
{
    RestoreRange rr(tr);
    Token *token;
    if (popToken(token, tr) && token->isSymbol('\''))
    {
        DEBUG_PRINT("Found a character:");
        auto character = new Character;
        if (!popToken(token, tr))
            gubg::Exception::raise(gubg::Exception("Incomplete character literal found"));
        if (token->isSymbol('\\'))
        {
            character->add(token);
            if (!popToken(token, tr))
                gubg::Exception::raise(gubg::Exception("No token following an escape character"));
        }
        character->add(token);
        if (!popToken(token, tr) || !token->isSymbol('\''))
            gubg::Exception::raise(gubg::Exception("Incomplete character literal found"));
        DEBUG_PRINT(character->toString());
        rr.commit();
        return character;
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
    auto comment = Comment::construct(tr);
    return 0;
}
#endif
