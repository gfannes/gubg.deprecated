#ifndef meta_parsing_Component_hpp
#define meta_parsing_Component_hpp

#include <vector>

namespace meta
{
    struct Component
    {
    };
    struct Composite: Component
    {
        std::vector<Component*> childs_;
    };
}

#endif
