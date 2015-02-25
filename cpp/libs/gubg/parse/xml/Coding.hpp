#ifndef HEADER_gubg_parse_xml_Coding_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_xml_Coding_hpp_ALREADY_INCLUDED

#include "gubg/parse/xml/Codes.hpp"
#include <string>

namespace gubg { namespace xml {

    ReturnCode encode(std::string &enc, const std::string &raw);
    inline std::string encode(const std::string &raw){std::string enc; encode(enc, raw); return enc;}
    enum QuoteStrategy {DisallowQuote, AllowQuote};
    ReturnCode decode(std::string &raw, const std::string &enc, QuoteStrategy qs = DisallowQuote);

} }

#endif
