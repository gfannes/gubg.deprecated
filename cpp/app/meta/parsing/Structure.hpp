#ifndef meta_parsing_Structure_hpp
#define meta_parsing_Structure_hpp

#include "meta/parsing/Token.hpp"
#include "meta/parsing/Component.hpp"
#include "boost/range/iterator_range.hpp"
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
        typedef std::vector<Token*> Tokens;
        Tokens tokens_;
        typedef std::vector<Component*> Components;
        Components components_;
    };
    typedef boost::iterator_range<Structure::Tokens::iterator> TokenRange;
}

#endif
