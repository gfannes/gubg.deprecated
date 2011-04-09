#ifndef meta_parsing_Component_hpp
#define meta_parsing_Component_hpp

#include <vector>
#include <string>

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
    };
    //A Composite consisting of _only_ Tokens
    class Token;
    struct TokenComposite: Component
    {
        std::vector<Token*> childs_;
        void add(Token *token){childs_.push_back(token);}
        std::string toString() const;
    };
    //A Composite consists of multiple Components
    struct Composite: Component
    {
        std::vector<Component*> childs_;
        void add(Component *component){childs_.push_back(component);}
    };
}

#endif
