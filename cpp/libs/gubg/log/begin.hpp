#ifndef HEADER_gubg_log_begin_hpp_ALREADY_INCLUDED
#define HEADER_gubg_log_begin_hpp_ALREADY_INCLUDED

#ifdef S
#error S is already defined
#endif
#ifdef L
#error L is already defined
#endif

#ifdef GUBG_MODULE
#include "gubg/log/macros.hpp"
#else
#include "gubg/log/macros_noop.hpp"
#endif

#endif
