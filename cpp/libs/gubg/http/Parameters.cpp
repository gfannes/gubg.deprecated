#include "gubg/http/Parameters.hpp"
#include "gubg/String.hpp"

namespace gubg { namespace http { 

    bool priv::CaseInsensitive::operator()(const Key &lhs, const Key &rhs) const
    {
        Key l(lhs); gubg::upcase(l);
        Key r(rhs); gubg::upcase(r);
        return l < r;
    }

} } 
