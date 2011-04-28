#include "parsing/Composite.hpp"
#include "parsing/Parser.hpp"
#include "OnlyOnce.hpp"
#include <sstream>
using namespace meta;
using namespace std;

#define L_ENABLE_DEBUG
#include "debug.hpp"

namespace
{
    template <typename Range, typename Ptr>
    struct RestoreRange
    {
        RestoreRange(Range &r, Ptr &ptr):
            orig_(r),
            ref_(r),
            ptr_(ptr){}
        ~RestoreRange()
        {
            if (!ptr_)
                ref_ = orig_;
        }

        Range orig_;
        Range &ref_;
        Ptr &ptr_;
    };
    template <typename Ptr>
    struct RestoreTokenRange: private RestoreRange<TokenRange::Range, Ptr>
    {
        typedef RestoreRange<TokenRange::Range, Ptr> BaseT;
        RestoreTokenRange(TokenRange &tr, Ptr &ptr):
            BaseT(tr.range, ptr){}
    };
    template <typename Ptr>
    struct RestoreComponentRange: private RestoreRange<ComponentRange, Ptr>
    {
        typedef RestoreRange<ComponentRange, Ptr> BaseT;
        RestoreComponentRange(ComponentRange &cr, Ptr &ptr):
            BaseT(cr, ptr){}
    };
}

string TokenComposite::toString() const
{
    ostringstream oss;
    for (auto &v: childs_)
        oss << toCode(v->range_);
    return oss.str();
}

string Composite::toString() const
{
    ostringstream oss;
    for (auto &v: childs_)
        oss << v->toString();
    return oss.str();
}
Comment::Ptr Comment::construct(TokenRange &tr)
{
    Ptr comment;
    RestoreTokenRange<Ptr> rr(tr, comment);
    Token::Ptr token;
    if (popToken(token, tr) && token->isSymbol('/') && popToken(token, tr) && token->isSymbol('/'))
    {
        comment.reset(new Comment);
        DEBUG_PRINT("Found a comment: " << comment.get());
        while (popToken(token, tr))
        {
            comment->add(token);
            if (token->isNewline())
                break;
        }
        DEBUG_PRINT(comment->toString());
    }
    return comment;
}

Include::Ptr Include::construct(TokenRange &tr)
{
    Ptr incl;
    RestoreTokenRange<Ptr> rr(tr, incl);
    Token::Ptr token;
    char lch;
    if (popToken(token, tr) && token->isSymbol('#') &&
            popToken(token, tr) && token->isName("include") &&
            popToken(token, tr) && token->isWhitespace() && 
            popToken(token, tr) && token->isSymbol(lch, "\"<"))
    {
        DEBUG_PRINT("Found an include:");
        incl.reset(new Include);
        while (popToken(token, tr))
        {
            char rch;
            if (token->isSymbol(rch, "\">") && (('\"' == lch && '\"' == rch) || ('<' == lch && '>' == rch)))
            {
                //We found the closing '"' or '>' symbol
                DEBUG_PRINT(incl->toString());
                return incl;
            }
            incl->add(token);
        }
        incl.reset();
        gubg::Exception::raise(gubg::Exception("Could not find the closing tag for an include statement"));
    }
    return incl;
}

Define::Ptr Define::construct(TokenRange &tr)
{
    Ptr def;
    RestoreTokenRange<Ptr> rr(tr, def);
    Token::Ptr token;
    if (popToken(token, tr) && token->isSymbol('#') &&
            popToken(token, tr) && token->isName("define") &&
            popToken(token, tr) && token->isWhitespace())
    {
        DEBUG_PRINT("Found a define:");
        def.reset(new Define);
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
    }
    return def;
}

String::Ptr String::construct(TokenRange &tr)
{
    Ptr str;
    RestoreTokenRange<Ptr> rr(tr, str);
    Token::Ptr token;
    if (popToken(token, tr) && token->isSymbol('\"'))
    {
        DEBUG_PRINT("Found a string:");
        str.reset(new String);
        while (popToken(token, tr))
        {
            char ch;
            if (token->isSymbol(ch, "\\\""))
            {
                switch (ch)
                {
                    case '\"':
                        DEBUG_PRINT(str->toString());
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
        str.reset();
        gubg::Exception::raise(gubg::Exception("Could not find the closing quote for an inline string"));
    }
    return str;
}

Character::Ptr Character::construct(TokenRange &tr)
{
    Ptr character;
    RestoreTokenRange<Ptr> rr(tr, character);
    Token::Ptr token;
    if (popToken(token, tr) && token->isSymbol('\''))
    {
        DEBUG_PRINT("Found a character:");
        character.reset(new Character);
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
    }
    return character;
}

Namespace::Ptr Namespace::construct(ComponentRange &cr)
{
    Ptr ns;
    RestoreComponentRange<Ptr> rr(cr, ns);
    Token::Ptr token;
    if (popToken(token, cr) && token->isName("namespace"))
    {
        ns.reset(new Namespace);
        if (popToken(token, cr) &&
                (!token->getName(ns->name_) || popToken(token, cr)) &&//"namespace" can be followed by a name
                token->isSymbol('{'))
        {
            DEBUG_PRINT("Found a namespace " << ns->name_);
            ns->childs_ = Parser::parseBlock(cr);
            DEBUG_PRINT("Namespace body: " << ns->toString());
        }
        else
                gubg::Exception::raise(gubg::Exception("I expect a block after namespace"));
    }
    return ns;
}

#ifdef UnitTest
int main()
{
    Code code("//bla bli blo\nflap");
    CodeRange cr(code);
    Tokens tokens;
    tokens.push_back(Token::Ptr(new Symbol(reduce(cr, 1))));
    tokens.push_back(Token::Ptr(new Symbol(reduce(cr, 1))));
    tokens.push_back(Token::Ptr(new Name(reduce(cr, 3))));
    tokens.push_back(Token::Ptr(new Whitespace(reduce(cr, 1))));
    tokens.push_back(Token::Ptr(new Name(reduce(cr, 3))));
    tokens.push_back(Token::Ptr(new Whitespace(reduce(cr, 1))));
    tokens.push_back(Token::Ptr(new Name(reduce(cr, 3))));
    tokens.push_back(Token::Ptr(new Newline(reduce(cr, 1))));
    tokens.push_back(Token::Ptr(new Name(reduce(cr, 4))));
    TokenRange tr(tokens);
    auto comment = Comment::construct(tr);
    return 0;
}
#endif
