#ifndef meta_parsing_Structure_hpp
#define meta_parsing_Structure_hpp

#include "parsing/Token.hpp"
#include "parsing/Component.hpp"
#include <list>

namespace meta
{
    struct Structure
    {
        Structure(){}
        Structure(Code &code);
        virtual ~Structure();
        Structure(const Structure &);
        Structure(Structure &&);
        Structure &operator=(Structure &&);

        std::list<std::string> allNames() const;

        std::string code_;
        Tokens tokens_;
        typedef std::list<Component*> Components;
        Components components_;

        private:
        Structure &operator=(const Structure &);
    };
}

#endif
