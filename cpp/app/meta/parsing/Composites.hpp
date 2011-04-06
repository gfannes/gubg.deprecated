#ifndef meta_parsing_Composites_hpp
#define meta_parsing_Composites_hpp

#include "parsing/Component.hpp"
#include "parsing/Token.hpp"

namespace meta
{
    struct Comment: TokenComposite
    {
        static Comment *tryCreate(TokenRange &tokens);
    };
}

#endif
