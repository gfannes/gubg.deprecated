#ifndef HEADER_gubg_http_Parameters_hpp_ALREADY_INCLUDED
#define HEADER_gubg_http_Parameters_hpp_ALREADY_INCLUDED

#include <string>
#include <map>

namespace gubg { namespace http { 

    typedef std::string Key;
    typedef std::string Value;

    namespace priv { 
        struct CaseInsensitive
        {
            bool operator()(const Key &lhs, const Key &rhs) const;
        };
    } 
    typedef std::map<Key, Value, priv::CaseInsensitive> Parameters;

} } 
#endif
