#ifndef meta_parsing_Structure_hpp
#define meta_parsing_Structure_hpp

#include "parsing/Token.hpp"
#include "parsing/Component.hpp"
#include <vector>

namespace meta
{
    struct Structure
    {
        Structure(Code &code);
        virtual ~Structure();
        Structure(const Structure &);
        Structure(Structure &&);

        std::list<std::string> allNames() const;

        Code code_;
        Tokens tokens_;
        typedef std::vector<Component*> Components;
        Components components_;

        private:
        Structure &operator=(const Structure &);
        Structure &operator=(Structure &&);
    };
}

#endif
