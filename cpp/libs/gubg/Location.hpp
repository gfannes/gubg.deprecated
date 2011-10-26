#ifndef gubg_Location_hpp
#define gubg_Location_hpp

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
#define GUBG_HERE() Location()
}

#endif
