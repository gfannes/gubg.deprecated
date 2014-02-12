#ifndef HEADER_gubg_String_hpp_ALREADY_INCLUDED
#define HEADER_gubg_String_hpp_ALREADY_INCLUDED

#include <string>
#include <sstream>
#include <iomanip>

namespace gubg { 

    void upcase(std::string &str);

    template <typename String>
        std::string toHex(const String &binary)
        {
            std::ostringstream res;
            res << std::hex;
            for (size_t i = 0; i < binary.size(); ++i)
                res << "0x" << std::setw(2) << std::setfill('0') << (int)(0xff & binary[i]) << ", ";
            return res.str();
        }

} 

#endif
