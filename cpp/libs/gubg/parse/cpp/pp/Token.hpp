#ifndef HEADER_gubg_parse_cpp_Token_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_cpp_Token_hpp_ALREADY_INCLUDED

#include "gubg/parse/cpp/Types.hpp"
#include <ostream>

namespace gubg
{
    namespace parse
    {
        namespace cpp
        {
            namespace pp
            {
                struct Token
                {
                    enum Type {Macro, String, Character, Identifier, Blanks, Newline, CarriageReturn, Symbol, LineComment, BlockComment, StandaloneBackslashNewline};

                    Type type;
                    Range range;

                    std::string toString() const {return std::string(range.begin(), range.end());}
                };
            }
        }
    }
}
inline std::ostream &operator<<(std::ostream &os, const gubg::parse::cpp::pp::Token &token)
{
    return os << token.toString();
}

#endif
