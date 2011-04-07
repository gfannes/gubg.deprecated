#ifndef meta_parsing_Composites_hpp
#define meta_parsing_Composites_hpp

#include "parsing/Component.hpp"
#include "parsing/Token.hpp"

namespace meta
{
    //The static method tryCreate() tries to create a TokenComposite of a specific type.
    //If this fails, nullptr is returned.
    struct Comment: TokenComposite
    {
        static Comment *tryCreate(TokenRange &);
    };
    struct Include: TokenComposite
    {
        static Include *tryCreate(TokenRange &);
    };
    struct String: TokenComposite
    {
        static String *tryCreate(TokenRange &);
    };
}

#endif
