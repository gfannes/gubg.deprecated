#ifndef meta_parsing_Component_hpp
#define meta_parsing_Component_hpp

#include <memory>

namespace meta
{
    //Parsing will happen in a few layers:
    // * At the lowest layer, we have Tokens, basically some subrange of the Code
    // * At the next level, we have TokenComposites, we are aggregates of _only_ Tokens
    // * Next come the general Composites, which can consist of anything
    //
    //This is achieved with the Composite pattern. Component is the base class of all object type in the layers
    struct Component
    {
        typedef std::shared_ptr<Component> Ptr;

        //Component is polymorphic
        virtual ~Component(){}
    };
}

#endif
