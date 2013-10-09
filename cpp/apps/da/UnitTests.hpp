#ifndef HEADER_da_UnitTests_hpp_ALREADY_INCLUDED
#define HEADER_da_UnitTests_hpp_ALREADY_INCLUDED

#include "da/Tasks.hpp"

namespace da
{
    class UnitTests: public Task_itf
    {
        public:
           static Ptr create() { return Ptr(new UnitTests); }

            virtual ReturnCode execute(const Options &);
        private:
    };
}

#endif
