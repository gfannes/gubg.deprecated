#ifndef HEADER_meta_parsing_Token_hpp_ALREADY_INCLUDED
#define HEADER_meta_parsing_Token_hpp_ALREADY_INCLUDED

#include "parsing/Code.hpp"
#include "parsing/Component.hpp"
#include <list>
#include <memory>

namespace meta
{
    //A Token is the leaf Component
    struct Token: Component
    {
        typedef std::shared_ptr<Token> Ptr;

        Token(const CodeRange &range):
            range_(range){}

        virtual std::string toString() const {return toCode(range_) + "|";}

        virtual bool isEnd(){return false;}
        virtual bool isSymbol(const char wanted){return false;}
        virtual bool isSymbol(char &actual, const std::string &symbols){return false;}
        bool isNewline(){return isSymbol('\n');}
        virtual bool isName(const std::string &){return false;}
        virtual bool getName(std::string &){return false;}
        virtual bool isWhitespace() { return false; }

        static Token::Ptr construct(CodeRange &range);

        CodeRange range_;
    };

    typedef std::list<Token::Ptr> Tokens;
    struct TokenRange
    {
        TokenRange(Tokens &t):
            tokens(t),
            range(tokens){}

        bool empty() const {return range.empty();}
        void pop_front() {range.pop_front();}
        Token::Ptr front() const {return range.front();}

        Tokens &tokens;
        typedef boost::iterator_range<Tokens::iterator> Range;
        Range range;
    };
    //Returns true if a token (!= from End Token) could be popped
    bool popToken(Token::Ptr &token, TokenRange &tr);
    bool popToken(Token::Ptr &token, ComponentRange &cr);

    //Different types of Tokens
    struct End: Token
    {
        End(const CodeRange &range):
            Token(range){}
        virtual bool isEnd(){return true;}
    };
    struct Symbol: Token
    {
        Symbol(const CodeRange &range):
            Token(range){}
        
        virtual bool isSymbol(const char wanted)
        {
            return wanted == range_.front();
        }
        virtual bool isSymbol(char &actual, const std::string &symbols)
        {
            size_t pos = symbols.find(range_.front());
            if (std::string::npos == pos)
                return false;
            actual = symbols[pos];
            return true;
        }
    };
    struct Newline: Symbol
    {
        Newline(const CodeRange &range):
            Symbol(range){}
    };
    struct Name: Token
    {
        Name(const CodeRange &range):
            Token(range){}

        virtual bool isName(const std::string &str)
        {
            return str == toCode(range_);
        }
        virtual bool getName(std::string &str)
        {
            str = toCode(range_);
            return true;
        }
    };
    struct Number: Token
    {
        Number(const CodeRange &range):
            Token(range){}
    };
    struct Whitespace: Token
    {
        Whitespace(const CodeRange &range):
            Token(range){}

        virtual bool isWhitespace() { return true; }
    };
}

#endif
