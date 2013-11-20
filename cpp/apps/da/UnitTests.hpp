#ifndef HEADER_da_UnitTests_hpp_ALREADY_INCLUDED
#define HEADER_da_UnitTests_hpp_ALREADY_INCLUDED

#include "da/Tasks.hpp"
#include "gubg/Verbose.hpp"

namespace da
{
    class UnitTests: public Task_itf, public gubg::Verbose<true>
    {
        public:
           static Ptr create() { return Ptr(new UnitTests); }

           virtual ReturnCode execute(const Options &);
        private:
    };
}

#endif
