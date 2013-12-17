#ifndef HEADER_rinle_model_Types_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_Types_hpp_ALREADY_INCLUDED

#include "gubg/parse/cpp/pp/Lexer.hpp"
#include <string>
#include <list>
#include <cassert>

namespace rinle { namespace model {

    typedef gubg::parse::cpp::pp::Token Token;
    typedef std::list<Token> Tokens;
    struct Range
    {
        Tokens::iterator begin;
        Tokens::iterator end;

        Range(): begin(), end() {}
        Range(Tokens &tokens): begin(tokens.begin()), end(tokens.end()) {}
        Range(Tokens::iterator b, Tokens::iterator e): begin(b), end(e) {}

        bool empty() const { return begin == end; }
        Token &front()
        {
            assert(begin != end);
            return *begin;
        }
        void popFront()
        {
            assert(begin != end);
            ++begin;
        }
    };

} }

#endif
