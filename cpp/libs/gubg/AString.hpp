#ifndef HEADER_gubg_AString_hpp_ALREADY_INCLUDED
#define HEADER_gubg_AString_hpp_ALREADY_INCLUDED

#include <string>
#include <bitset>

namespace gubg
{
    template <typename Flags>
        class AString: public std::string
        {
            public:
                std::bitset<(unsigned long)Flags::Nr> flags;

                AString() {}
                AString(const char *cstr): std::string(cstr) {}
                AString(const std::string &str): std::string(str) {}
        };
}

#endif
