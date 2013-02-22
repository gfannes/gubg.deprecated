#ifndef HEADER_da_Context_hpp_ALREADY_INCLUDED
#define HEADER_da_Context_hpp_ALREADY_INCLUDED

#include "da/platform/Host.hpp"
#include "da/platform/Target.hpp"
#include "gubg/Context.hpp"

namespace da
{
    struct Context
    {
        platform::Host host;
        platform::Target target;
    };
}

#endif
