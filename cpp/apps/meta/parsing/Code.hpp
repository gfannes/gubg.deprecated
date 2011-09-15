#ifndef meta_parsing_Code_hpp
#define meta_parsing_Code_hpp

#include "gubg/Exception.hpp"
#include "boost/range/iterator_range.hpp"

namespace meta
{
    typedef const std::string Code;
    typedef boost::iterator_range<Code::const_iterator> CodeRange;

    //Some convenience functions for working with a CodeRange
    //
    struct OutOfRange: gubg::Exception
    {
        OutOfRange():
            gubg::Exception("Range is not big enough"){}
    };
    //Returns a range of length "nr" taken from the front of "range" and modifies range to take the rest
    //Throws meta::OutOfRange when range is not big enough
    CodeRange reduce(CodeRange &range, unsigned int nr) throw (OutOfRange);
    //
    Code toCode(const CodeRange &cr);
}

#endif
