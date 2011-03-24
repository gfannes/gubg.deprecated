#ifndef meta_parsing_Structure_hpp
#define meta_parsing_Structure_hpp

#include <vector>
#include <string>

namespace meta
{
    struct Component
    {
    };
    struct Composite: Component
    {
        std::vector<Component*> childs_;
    };
    struct Token: Component
    {
        const char *start_;
    };
    struct Symbol: Token
    {
    };
    struct Newline: Symbol
    {
    };
    struct Name: Token
    {
    };
    struct Whitespace: Token
    {
    };
    struct Structure
    {
        Structure(const std::string &code);
        virtual ~Structure();

        std::string code_;
        std::vector<Token*> tokens_;
        std::vector<Component*> components_;
    };
}

#endif
