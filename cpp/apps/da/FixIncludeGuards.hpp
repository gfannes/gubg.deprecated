#ifndef HEADER_da_FixIncludeGuards_hpp_ALREADY_INCLUDED
#define HEADER_da_FixIncludeGuards_hpp_ALREADY_INCLUDED

#include "da/Tasks.hpp"

namespace da
{
    class FixIncludeGuards: public Task_itf
    {
        public:
            static Ptr create(){return Ptr(new FixIncludeGuards);}
            virtual ReturnCode execute(const Options &);
    };
}

#endif
