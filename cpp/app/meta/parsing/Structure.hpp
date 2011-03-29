#ifndef meta_parsing_Structure_hpp
#define meta_parsing_Structure_hpp

#include "meta/parsing/Token.hpp"
#include "meta/parsing/Component.hpp"
#include <vector>
#include <string>

namespace meta
{
    struct Structure
    {
        Structure(const std::string &code);
        virtual ~Structure();
        Structure(const Structure &);
        Structure(Structure &&);
        Structure &operator=(const Structure &);
        Structure &operator=(Structure &&);

        std::string code_;
        std::vector<Token*> tokens_;
        std::vector<Component*> components_;
    };
}

#endif
