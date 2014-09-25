#ifndef HEADER_gubg_dynlib_Plugin_hpp_ALREADY_INCLUDED
#define HEADER_gubg_dynlib_Plugin_hpp_ALREADY_INCLUDED

#include "gubg/dynlib/Visibility.hpp"

#define GUBG_DYNLIB_SHARE(klass) \
    extern "C" { GUBG_DYNLIB_EXPORT void *create() { return new klass; } }

#endif
