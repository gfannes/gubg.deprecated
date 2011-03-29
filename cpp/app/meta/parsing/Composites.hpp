#ifndef meta_parsing_Composites_hpp
#define meta_parsing_Composites_hpp

#include "parsing/Component.hpp"
#include "parsing/Structure.hpp"

namespace meta
{
    struct Comment: Composite
    {
        static Comment *tryCreate(TokenRange &tokens);
    };
}

#endif
