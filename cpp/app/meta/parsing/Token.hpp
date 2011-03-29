#ifndef meta_parsing_Token_hpp
#define meta_parsing_Token_hpp

#include "parsing/Component.hpp"

namespace meta
{
    struct Token: Component
    {
        Token(const char *start, const char *end):
            start_(start),
            end_(end){}

        virtual bool isEnd(){return false;}

        static Token *tryCreate(const char *&ch);

        const char *start_;
        const char *end_;
    };
    struct End: Token
    {
        End(const char *ch):
            Token(ch, ch){}

        virtual bool isEnd(){return true;}
    };
    struct Symbol: Token
    {
        Symbol(const char *ch):
            Token(ch, ch+1){}
    };
    struct Newline: Symbol
    {
        Newline(const char *ch):
            Symbol(ch){}
    };
    struct Name: Token
    {
        Name(const char *start, const char *end):
            Token(start, end){}
    };
    struct Number: Token
    {
        Number(const char *start, const char *end):
            Token(start, end){}
    };
    struct Whitespace: Token
    {
        Whitespace(const char *start, const char *end):
            Token(start, end){}
    };
}

#endif
