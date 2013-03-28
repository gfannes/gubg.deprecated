//NO INCLUDE GUARDS HERE

#ifdef S
#error S is already defined
#endif
#ifdef SS
#error SS is already defined
#endif
#ifdef L_S__
#error L_S__ is already defined
#endif
#ifdef L
#error L is already defined
#endif
#ifdef LL
#error LL is already defined
#endif
#ifdef L_L__
#error L_L__ is already defined
#endif

#ifdef GUBG_MODULE
#include "gubg/log/macros.hpp"
#else
#include "gubg/log/macros_noop.hpp"
#endif
