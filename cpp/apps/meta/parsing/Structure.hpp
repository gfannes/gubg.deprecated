#ifndef meta_parsing_Structure_hpp
#define meta_parsing_Structure_hpp

#include "parsing/Token.hpp"
#include "parsing/Component.hpp"
#include "boost/shared_ptr.hpp"
#include <list>
#include <map>

namespace meta
{
    struct Structure
    {
        typedef boost::shared_ptr<Structure> Ptr;

        Structure(Code &code);

        std::map<std::string, unsigned int> countPerName() const;

        std::string toString() const;

        Code code_;
        Tokens tokens_;
        Components components_;

        private:
        //Tokens keep iterators into code_, copying them will fail miserably
        Structure(const Structure &);
        Structure(Structure &&);
        Structure &operator=(Structure &&);
        Structure &operator=(const Structure &);
    };
}

#endif
