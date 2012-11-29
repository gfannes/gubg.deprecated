#ifndef HEADER_da_build_Headers_hpp_ALREADY_INCLUDED
#define HEADER_da_build_Headers_hpp_ALREADY_INCLUDED

#include "da/Codes.hpp"
#include "da/build/Header.hpp"
#include "gubg/Verbose.hpp"
#include <vector>

namespace da
{
    class Headers: gubg::Verbose<true>
    {
        public:
            ReturnCode add(Header::Ptr);

            typedef std::vector<Header::Ptr> Headers_;
            typedef Headers_::const_iterator const_iterator;
            const_iterator begin() const {return headers_.begin();}
            const_iterator end() const {return headers_.end();}

        private:
            Headers_ headers_;
    };
}

#endif
