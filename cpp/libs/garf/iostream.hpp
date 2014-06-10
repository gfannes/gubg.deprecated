#ifndef HEADER_garf_iostream_hpp_ALREADY_INCLUDED
#define HEADER_garf_iostream_hpp_ALREADY_INCLUDED

#include "gubg/OnlyOnce.hpp"

namespace garf { 

    class ostream;

    typedef ostream &(*IOManip_ptr)(ostream&);
    ostream &endl(ostream &);

    class ostream
    {
        public:
            ostream &operator<<(const char *cstr);
            ostream &operator<<(char ch);
            ostream &operator<<(int i);
            ostream &operator<<(unsigned long i);
            ostream &operator<<(IOManip_ptr);
            ostream &operator<<(void * const);

        private:
            gubg::OnlyOnce doInit_;
            void initialize_();
    };
    extern ostream cout;

} 

#endif
