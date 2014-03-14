#include "gubg/range/Concept.hpp"

namespace gubg { namespace range { 

    char f()
    {
        return 'a';
    }
    const char &g()
    {
        static char ch = 'b';
        return ch;
    }

} } 
