#ifndef HEADER_gubg_parse_cpp_pp_Token_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_cpp_pp_Token_hpp_ALREADY_INCLUDED

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
                    enum Type {Unknown, MacroHash, Macro, String, Character, Identifier, Blanks, CarriageReturn, LineFeed, Symbol, LineComment, BlockComment, StandaloneBackslashNewline};

                    Type type;
                    Range range;

                    std::string toString() const {return std::string(range.begin(), range.end());}
                    bool isWhitespace() const
                    {
                        switch (type)
                        {
                            case Token::LineComment:
                            case Token::BlockComment:
                            case Token::Blanks:
                            case Token::CarriageReturn:
                            case Token::LineFeed:
                                return true;
                                break;
                        }
                        return false;
                    }
                };
		const char *toString(Token::Type type);
		template <typename Token>
			bool isMacro(const Token &token){return token.type == Token::Macro;}
		template <typename Token>
			bool isIdentifier(const Token &token){return token.type == Token::Identifier;}
		template <typename Token>
			bool isString(const Token &token){return token.type == Token::String;}
		template <typename Token>
			bool isSymbol(const Token &token){return token.type == Token::Symbol;}
		template <typename Token>
			bool isComment(const Token &token){return token.type == Token::Comment;}
		template <typename Token>
			bool isLineFeed(const Token &token){return token.type == Token::LineFeed;}
		template <typename Token>
			bool isCarriageReturn(const Token &token){return token.type == Token::CarriageReturn;}
            }
        }
    }
}
inline std::ostream &operator<<(std::ostream &os, const gubg::parse::cpp::pp::Token &token)
{
    return os << token.toString();
}

#endif
