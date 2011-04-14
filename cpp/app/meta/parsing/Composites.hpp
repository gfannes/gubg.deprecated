#ifndef meta_parsing_Composites_hpp
#define meta_parsing_Composites_hpp

#include "parsing/Component.hpp"
#include "parsing/Token.hpp"

namespace meta
{
    //The static method construct() tries to create a TokenComposite of a specific type.
    //If this fails, nullptr is returned.
    struct Comment: TokenComposite
    {
        static Comment *construct(TokenRange &);
    };
    struct Include: TokenComposite
    {
        static Include *construct(TokenRange &);
    };
    struct Define: TokenComposite
    {
        static Define *construct(TokenRange &);
    };
    struct String: TokenComposite
    {
        static String *construct(TokenRange &);
    };
    struct Character: TokenComposite
    {
        static Character *construct(TokenRange &);
    };
}

#endif
