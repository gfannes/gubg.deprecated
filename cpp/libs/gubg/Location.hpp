#ifndef HEADER_gubg_Location_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Location_hpp_ALREADY_INCLUDED

#include <ostream>

#define GUBG_HERE() gubg::Location(__FILE__, __LINE__)

namespace gubg
{
    struct Location
    {
        Location():
            file(0), nr(0){}
        Location(const char *f, int n):
            file(f), nr(n){}
        const char *file;
        int nr;
    };
}
inline std::ostream &operator<<(std::ostream &os, const gubg::Location &location)
{
    if (!location.file)
        return os << "Unknown location";
    return os << location.file << ":" << location.nr;
}

#endif
