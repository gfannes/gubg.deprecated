#ifndef HEADER_da_FixNewLines_hpp_ALREADY_INCLUDED
#define HEADER_da_FixNewLines_hpp_ALREADY_INCLUDED

#include "da/Tasks.hpp"

namespace da
{
    class FixNewlines: public Task_itf
    {
        public:
            static Ptr create(){return Ptr(new FixNewlines);}
            virtual ReturnCode execute(const Options &);
    };
}

#endif
